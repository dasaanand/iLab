/*
 *  main.cpp
 *  iLabServer
 *
 *  Created by Dasa Anand on 19/02/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "headers.h"


float xy_aspect;
int   last_x, last_y;

/** These are the live variables passed into GLUI ***/
int main_window;
int capScreen_radioValue;


/** Pointers to the windows and some of the controls we'll create **/
GLUI *glui, *messageWindow;
GLUI_Panel *login_panel, *capScreen_panel, *message_panel, *evaluation_panel;
GLUI_RadioGroup *radio;
GLUI_TextBox *message;

/** Identifiers for connection **/
int sockfd, newfd[BACKLOG], ret_bind, theiraddr_size, recv_bytes, send_bytes, yes = 1;
struct sockaddr_in myaddr;
char msg[MESSAGE_SIZE];
int connected = 0, fdmax;
fd_set master,read_fds;
int pid;
std::string consoleText = "";


struct students_info
{
	int newfd;
	struct sockaddr_in theiraddr;
	char username[20], password[20];
}studentInfo[BACKLOG];


void textDisplay(std::string text)
{	
	
	glDisable( GL_LIGHTING );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0.0, 100.0, 0.0, 100.0  );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glColor3ub( 0, 0, 0 );
	glRasterPos2i( GLUT_WINDOW_WIDTH - 100, GLUT_WINDOW_HEIGHT - 10);	
	int i;
	
	glClearColor( .9f, .9f, .9f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glutSwapBuffers(); 
	
	for( i=0; i<text.length(); i++ )
		glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18,text[i]);
	
	glEnable( GL_LIGHTING );
	glutSwapBuffers();
}



void verifyUser(int i)
{
	
	FILE * pFile;

	pFile = fopen ( "users.txt" , "r" ); 		// Open file
	
	int success = 0;
	int a,b;
	
	do {
		char name[20], pass[20];
		
		a = fscanf(pFile, "%s", name);
		if (a != EOF) {
			b = fscanf(pFile, "%s", pass);
			if ((strcmp(name, studentInfo[i].username) == 0) && (strcmp(pass, studentInfo[i].password) == 0)) {
				success = 1;
			}
		}
	}while (success != 1 && a != EOF);
	
	
	fclose(pFile);
	
	if (success == 0) {
		strcpy(msg, HeaderInvalid);
		strcat(msg, "Invalid user");
	}
	else {
		strcpy(msg, HeaderValid);
		strcat(msg, "Valid user");
	}

}


void readAttendance(int i)
{
	FILE * pFile;
	
	pFile = fopen ( "attendance.txt" , "r" ); 		// Open file
	
	int success = 0;
	int a,b;
	char name[20], att[20];
	
	do {
		
		a = fscanf(pFile, "%s", name);
		if (a != EOF) {

			if ((strcmp(name, studentInfo[i].username) == 0)) 
			{
				b = fscanf(pFile, "%s", att);
				success = 1;
			}
		}
	}while (success != 1 && a != EOF);
	
	
	fclose(pFile);
	
	if (success == 1) {
		strcpy(msg, HeaderAttendance);
		strcat(msg, att);
	}
}


/**************************************** connectTo() **********/
void connectTo()
{
	
	int i,k;
	char filename[20];
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	
	
	/************************* Initialize connection *************************/
	
	/* Get a socket file descriptor */
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	
	if(sockfd == -1)
		perror("socket");
	else
		textDisplay("Server started");

	int x = ioctl(sockfd, FIONBIO, 1);
	
	
	/* Enable reuse of the port number immediately after program
	 * termination.
	 */
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		perror("setsockopt");

	
	myaddr.sin_family = AF_INET;					/* Host byte order */
	myaddr.sin_port = htons(SERVER_PORT);			/* Network byte order */
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY); 	/* Get my IP address */
	memset(&(myaddr.sin_zero), '\0', 8); 			/* Zero it out */
	
	
	/* Bind it to your IP address and a suitable port number. */
	ret_bind = bind(sockfd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr));
	
	if(ret_bind == -1)
		perror("bind");
	
	

	
	/* Try listening to incoming connection requests.
	 * And set a limit on the size of the connection queue.
	 */
	if(listen(sockfd, BACKLOG) == -1)
		perror("listen");
	
	
	FD_SET(sockfd, &master);	//adding the main socket to the master set
	fdmax = sockfd;
	
	pid = fork();
	if (pid == 0)
	{
		while(1)
		{
			read_fds = master; // copy it 
			if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) 
			{
				perror("select"); 
				exit(1);
			}
			
			// run through the existing connections looking for data to read 
			for(i = 0; i <= fdmax; i++) 
			{
				if (FD_ISSET(i, &read_fds)) 
				{
					// we got one!! 
					if (i == sockfd)
					{
						// handle new connections 
						connected++;
						theiraddr_size = sizeof(struct sockaddr_in);
						if ((studentInfo[connected].newfd = accept(sockfd,  (struct sockaddr *)&studentInfo[connected].theiraddr, (socklen_t *)&theiraddr_size)) == -1)
							perror("accept");
						else
						{
							FD_SET(studentInfo[connected].newfd, &master);
							if (studentInfo[connected].newfd > fdmax)
							{
								fdmax = studentInfo[connected].newfd;
							}
							printf("selectserver: new connection from %s on socket %d\n", inet_ntoa(studentInfo[connected].theiraddr.sin_addr), studentInfo[connected].newfd);
						
						}
					}
					else 
					{
						if ((recv_bytes = recv(i, (void *)msg, sizeof(msg), 0)) <= 0) 
						{
							if (recv_bytes == 0) 
							{
								printf("Connection closed for %d\n",i);
							}
							else 
							{
								perror("recv");
							}
							close(i);
							FD_CLR(i, &master);
						}
						else 
						{
							msg[recv_bytes] = '\0';
							printf("\nReceived: %s from %d\n",msg, i);
							
							if (strstr(msg, HeaderUsername) != NULL)
							{
								printf("Username:\n");
								strcpy(studentInfo[i].username, " ");
								studentInfo[i].username[0] = '\0';
								
								for (int k = 4; k < strlen(msg); k++)
								{
									studentInfo[i].username[k-4] = msg[k];
								}
							}
							else
							{
								if (strstr(msg, HeaderPassword) != NULL)
								{
									printf("Password:\n");
									strcpy(studentInfo[i].password, " ");
									studentInfo[i].password[0] = '\0';
									
									for (k = 4; k < strlen(msg); k++)
									{
										studentInfo[i].password[k-4] = msg[k];
									}
									verifyUser(i);
								}
								if (strstr(msg, HeaderAttendance) != NULL)
								{
									printf("Attendance:\n");
									readAttendance(i);
								}
								else
								{
									if (strstr(msg, HeaderMessage) != NULL)
									{
										printf("Message:\n");
										char *message1;
										for (k = 4; k < strlen(msg); k++)
										{
											message1[k-4] = msg[k];
										}
										printf("%s\n",message1);
										
									}
									else
									{
										printf("called1");
										if (strstr(msg, HeaderFileUpload) != NULL) 
										{
											printf("called2");
											printf("File received:\n");
											char *message1;
											for (k = 4; k < strlen(msg); k++)
											{
												message1[k-4] = msg[k];
											}
											
											FILE * pFile;
											strcpy(filename, studentInfo[i].username);
											strcat(filename, ".txt");
											printf("\n---- %s  ----- ", filename);
											pFile = fopen (filename , "w" ); 		// Open file
											fprintf(pFile, "%s\n", message1);
											fclose(pFile);
											strcpy(msg, HeaderFileUpload);
											strcat(msg, "ACK");
										}
									}
								}
							}

							//strcpy(msg, "ACK")
							send_bytes = send(i, (void *)msg, strlen(msg), 0);
							
							if(send_bytes != -1) {
								printf("ACK message to Client %d\n", i);
							} else {
								perror("send");	
								exit(1);
							}
						}
					}
				}
			}
		}		
	}
	
}


void message_cb(GLUI_Control* control)
{
	if (control->get_id() == closeLabel)
	{
		control->glui->close();
	}
	else
	{
		if (control->get_id() == showAttendance) 
		{
			FILE * pFile;
			long lSize;
			size_t result;
			char *buffer;
			
			pFile = fopen ( "attendance.txt" , "r" );
			
			fseek (pFile , 0 , SEEK_END);			// Seek end of file
			lSize = ftell (pFile);				// Calculate length of file
			rewind (pFile);					// Go back to start
			
			buffer = (char*) malloc (sizeof(char)*lSize);	// Allocate memory to read file
			result = fread (buffer,1,lSize,pFile);		// Read file
			fclose(pFile);
			
			message->set_text(buffer);
		}
		else if (control->get_id() == showLogin)
		{
				FILE * pFile;
				long lSize;
				size_t result;
				char *buffer;
				
				pFile = fopen ( "users.txt" , "r" );
				
				fseek (pFile , 0 , SEEK_END);			// Seek end of file
				lSize = ftell (pFile);				// Calculate length of file
				rewind (pFile);					// Go back to start
				
				buffer = (char*) malloc (sizeof(char)*lSize);	// Allocate memory to read file
				result = fread (buffer,1,lSize,pFile);		// Read file
				fclose(pFile);
				
				message->set_text(buffer);
		}
		else
		{
			if (control->get_id() == message_sendtoAll)
			{
				for (int i=0; i<connected; i++)
				{
					printf("called");
					send_bytes = send(studentInfo[i].newfd, (void *)msg, strlen(msg), 0);
					
					if(send_bytes != -1) {
						//printf("ACK message to Client %d\n", i);
					} else {
						perror("send");	
						exit(1);
					}
				}
			}
		}

	}

}



/**************************************** control_cb() *******************/
/* GLUI control callback                                                 */

void control_cb( int control )
{	
	switch (control)
	{
		case connectServer:
			connectTo();
			break;
		case disconnectServer:
			/******************* Release all resources ********************/
			for (int i = 0; i < connected; i++)
			{
				close(newfd[i]);
			}
			printf("Server connection is closed");
			close(sockfd);
			break;
		case login_attendance:
		{	
			printf("login attendance");
			
			messageWindow = GLUI_Master.create_glui( "Attendance:",0, 80, 400 );
			
			GLUI_Panel *panel = new GLUI_Panel(messageWindow,"", GLUI_PANEL_NONE);
			message = new GLUI_TextBox(panel,true);
			message->set_text("");
			message->set_h(300);
			message->set_w(310);
			message->enable();
			
			new GLUI_Button(panel, "Display", showAttendance, message_cb);
			new GLUI_Button(panel, "Close", closeLabel, message_cb);
			
			messageWindow->set_main_gfx_window( main_window );
		}
			
			break;
		case login_password:
		{
			printf("login_password");
			messageWindow = GLUI_Master.create_glui( "LoginDetails:",0, 80, 400 );
			
			GLUI_Panel *panel = new GLUI_Panel(messageWindow,"", GLUI_PANEL_NONE);
			message = new GLUI_TextBox(panel,true);
			message->set_text("");
			message->set_h(300);
			message->set_w(310);
			message->enable();
			
			new GLUI_Button(panel, "Display", showLogin, message_cb);
			new GLUI_Button(panel, "Close", closeLabel, message_cb);
			
			messageWindow->set_main_gfx_window( main_window );
		}
			
			break;
		case capScreen_student:
			break;
		case capScreen_radio:
			break;
		case capScreen_capture:
			printf("\n Captured: %d", radio->get_int_val());
			break;
		case message_create:
		{
			messageWindow = GLUI_Master.create_glui( "Message Create:",0, 80, 400 );
			
			GLUI_Panel *panel = new GLUI_Panel(messageWindow,"", GLUI_PANEL_NONE);
			message = new GLUI_TextBox(panel,true);
			message->set_text("");
			message->set_h(300);
			message->set_w(310);
			message->enable();
			
			new GLUI_Button(panel, "Broadcast", message_sendtoAll, message_cb);
			new GLUI_Button(panel, "Close", closeLabel, message_cb);
			
			messageWindow->set_main_gfx_window( main_window );
		}

			break;
		case message_sendto:
			break;
		case message_sendtoAll:
			break;
		case message_restart:
			break;
		case message_shutdown:
			break;
		case evaluate_student:
			break;
		case evaluate:
			break;
		default:
			break;
	}
}

/**************************************** myGlutKeyboard() **********/

void myGlutKeyboard(unsigned char Key, int x, int y)
{
	switch(Key)
	{
		case 27: 
		case 'q':
			exit(0);
			break;
	};
	
	glutPostRedisplay();
}


/***************************************** myGlutMenu() ***********/

void myGlutMenu( int value )
{
	myGlutKeyboard( value, 0, 0 );
}


/***************************************** myGlutIdle() ***********/

void myGlutIdle( void )
{
	/* According to the GLUT specification, the current window is 
     undefined during an idle callback.  So we need to explicitly change
     it if necessary */
	if ( glutGetWindow() != main_window ) 
		glutSetWindow(main_window);  
	
	/*  GLUI_Master.sync_live_all();  -- not needed - nothing to sync in this application  */
	
	glutPostRedisplay();
}

/***************************************** myGlutMouse() **********/

void myGlutMouse(int button, int button_state, int x, int y )
{
}


/***************************************** myGlutMotion() **********/

void myGlutMotion(int x, int y )
{
	glutPostRedisplay(); 
}

/**************************************** myGlutReshape() *************/

void myGlutReshape( int x, int y )
{
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
	glViewport( tx, ty, tw, th );
	
	xy_aspect = (float)tw / (float)th;
	
	glutPostRedisplay();
}



/***************************************** myGlutDisplay() *****************/

void myGlutDisplay( void )
{
	glClearColor( .9f, .9f, .9f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	consoleText = "This is the app for the teacher";	
	textDisplay(consoleText);
	glutSwapBuffers(); 
}


/**************************************** main() ********************/

int main(int argc, char* argv[])
{
	/****************************************/
	/*   Initialize GLUT and create window  */
	/****************************************/
	
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowPosition( 50, 50 );
	glutInitWindowSize( 800, 600 );
	
	main_window = glutCreateWindow( "iLab Teacher" );
	glutDisplayFunc( myGlutDisplay );
	GLUI_Master.set_glutReshapeFunc( myGlutReshape );  
	GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
	GLUI_Master.set_glutSpecialFunc( NULL );
	GLUI_Master.set_glutMouseFunc( myGlutMouse );
	

	/*** Create the side subwindow ***/
	glui = GLUI_Master.create_glui_subwindow( main_window, GLUI_SUBWINDOW_RIGHT );
	
	
	new GLUI_Button( glui, "Connect", connectServer, control_cb);

	
	/****************************************/
	/*   Login Panel init and adding objs   */
	/****************************************/	
	login_panel = new GLUI_Rollout(glui, "Login Details", false );
	new GLUI_Button(login_panel, "Attendance", login_attendance, control_cb);
	new GLUI_Button(login_panel, "Password", login_password, control_cb);
	
	
	/*********************************************/
	/* Capture screen Panel init and adding objs */
	/*********************************************/	
/*	capScreen_panel = new GLUI_Rollout(glui, "Capture Screen", false );
	new GLUI_Button(capScreen_panel, "Select student", capScreen_student, control_cb);
	new GLUI_StaticText(capScreen_panel, "No. of screenshots");
	radio = glui->add_radiogroup_to_panel(capScreen_panel, &capScreen_radioValue, capScreen_radio, control_cb);
	radio->set_alignment(GLUI_ALIGN_CENTER);
	new GLUI_RadioButton(radio, "3");
	new GLUI_RadioButton(radio, "4");
	new GLUI_RadioButton(radio, "5");
	new GLUI_Button(capScreen_panel, "Capture Screen", capScreen_capture, control_cb);*/
	
		
	/**************************************/
	/* Message Panel init and adding objs */
	/**************************************/	
	//message_panel = new GLUI_Rollout(glui, "Message", false );
	//new GLUI_Button(message_panel, "Create", message_create, control_cb);
	//new GLUI_Button(message_panel, "Send To", message_sendto, control_cb);
	//new GLUI_Button(message_panel, "Send To All", message_sendtoAll, control_cb);
	//new GLUI_Button(message_panel, "Restart", message_restart, control_cb);
	//new GLUI_Button(message_panel, "Shutdown", message_shutdown, control_cb);

	
	/**************************************/
	/* Evaluation Panel init and adding objs */
	/**************************************/	
//	evaluation_panel = new GLUI_Rollout(glui, "Evaluate", false );
//	new GLUI_Button(evaluation_panel, "Select student", evaluate_student, control_cb);
//	new GLUI_Button(evaluation_panel, "Evaluate", evaluate, control_cb);
	
	
	
	new GLUI_Button( glui, "Disconnect", disconnectServer, control_cb);
	new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );
	
	
	/**** Link windows to GLUI, and register idle callback ******/
	glui->set_main_gfx_window( main_window );
	
	#if 0
		/**** We register the idle callback with GLUI, *not* with GLUT ****/
		GLUI_Master.set_glutIdleFunc( myGlutIdle );
	#endif
	
	/**** Regular GLUT main loop ****/
	glutMainLoop();
	
	return EXIT_SUCCESS;
}




