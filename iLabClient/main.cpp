/*
 *  main.cpp
 *  iLabClient
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
GLUI *glui, *loginWindow, *warning, *messageWindow, *fileBrowser;
GLUI_Panel *login_panel, *capScreen_panel, *message_panel, *evaluation_panel;
GLUI_CommandLine *textBox, *fileName;
std::string consoleText = "";
GLUI_Button *username_btn, *password_btn, *attendance_btn;
GLUI_TextBox *message;
GLUI_FileBrowser *fb;
GLUI_List        *hah;


/** Identifiers for connection **/
int sockfd, ret_connect, recv_bytes, send_bytes, yes=1;
struct sockaddr_in server_addr;
char msg[MESSAGE_SIZE];
int pid, fdmax;
fd_set master,read_fds;


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

void login_cb(GLUI_Control* control)
{
	if(control->get_id() == login_username)
	{
		control->disable();
		loginWindow = GLUI_Master.create_glui( "Enter username:",0, 50, 500 );
		
		textBox = new GLUI_CommandLine(loginWindow, "User Name", NULL, -1, login_cb );
		textBox->set_w( 400 );  /** Widen 'command line' control **/
		
		GLUI_Panel *panel = new GLUI_Panel(loginWindow,"", GLUI_PANEL_NONE);
		new GLUI_Button(panel, "Send", loginW_username, login_cb);
		new GLUI_Column(panel, false);
		new GLUI_Button(panel, "Close", close_label , login_cb);
		
		loginWindow->set_main_gfx_window( main_window );
	}
	else 
	{
		if (control->get_id() == login_password) 
		{
			control->disable();
			loginWindow = GLUI_Master.create_glui( "Enter password:",0, 50, 500 );
			
			textBox = new GLUI_CommandLine(loginWindow, "Password", NULL, -1, login_cb );
			textBox->set_w( 400 );  /** Widen 'command line' control **/
			
			GLUI_Panel *panel = new GLUI_Panel(loginWindow,"", GLUI_PANEL_NONE);
			new GLUI_Button(panel, "Send", loginW_password, login_cb);
			new GLUI_Column(panel, false);
			new GLUI_Button(panel, "Close", close_label , login_cb);
			
			loginWindow->set_main_gfx_window( main_window );
		}
		else
		{
			if (control == textBox)
			{
				
			}
			else
			{
				if (control->get_id() == loginW_username)
				{
					strcpy(msg, HeaderUsername);
					strcat(msg, textBox->get_text());
					
					send_bytes = send(sockfd, (void *)msg, strlen(msg), 0);
					
					if(send_bytes != -1) {
						// printf("\n%s\n", msg);
						textDisplay("User name is sent");
					} else {
						perror("send");
						exit(1);
					}
					textBox->set_text("");	
					control->glui->close();
					username_btn->enable();
				}
				else
				{
					if (control->get_id() == close_label)
					{
						username_btn->enable();
						password_btn->enable();
						control->glui->close();
					}
					else
					{
						if (control->get_id() == loginW_password)
						{
							strcpy(msg, HeaderPassword);
							strcat(msg, textBox->get_text());	
							
							send_bytes = send(sockfd, (void *)msg, strlen(msg), 0);
							
							if(send_bytes != -1) {
								//printf("\n%s\n", msg);
								textDisplay("Password is sent");
							} else {
								perror("send");
								exit(1);
							}
							textBox->set_text("");
							control->glui->close();
							password_btn->enable();
						}
					}
				}
			}
		}
	}
}



void message_cb(GLUI_Control* control)
{
	if (control->get_id() == message_sendto)
	{
		strcpy(msg, HeaderMessage);
		strcat(msg, message->get_text());
		send_bytes = send(sockfd, (void *)msg, strlen(msg), 0);
		
		if(send_bytes != -1) {
			//printf("\n%s\n", msg);
			textDisplay("Message is sent");
		} else {
			perror("send");
			exit(1);
		}
		message->set_text(" ");
		control->glui->close();
	}
	else
	{
		control->glui->close();
	}

}


void fileBrowse_cb(GLUI_Control* control)
{
	if (control->get_id() == FBW_upload)
	{
		
		FILE * pFile;
		long lSize;
		size_t result;
		char *buffer;
		
		pFile = fopen ( fileName->get_text() , "r" ); 		// Open file
		
		fseek (pFile , 0 , SEEK_END);			// Seek end of file
		lSize = ftell (pFile);				// Calculate length of file
		rewind (pFile);					// Go back to start
		
		buffer = (char*) malloc (sizeof(char)*lSize);	// Allocate memory to read file
		result = fread (buffer,1,lSize,pFile);		// Read file
		
		fclose(pFile);
		strcpy(msg, HeaderFileUpload);
		strcat(msg,buffer);	

		send_bytes = send(sockfd, (void *)msg, strlen(msg), 0);
		
		if(send_bytes != -1) {
			textDisplay("File sent");
		} else 
			perror("send");
	}
	else
	{
		if (control->get_id() == FBW_close)
		{
			control->glui->close();
		}
		else
		{
			
		}

	}

}



/**************************************** connectTo() **********/
void connectTo()
{
	int k;

	/* ******************* Initialize connection *******************/
	
	/* Get a socket file descriptor */
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	
	if(sockfd == -1)
		perror("socket");
	else
	{
		textDisplay("Connected to the server");
	}

	/******* Enable reuse of the port number immediately after program termination ********/
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		perror("setsockopt");
	
	server_addr.sin_family = AF_INET; 				/* Host byte order */
	server_addr.sin_port = htons(SERVER_PORT);		/* Network byte order */
	server_addr.sin_addr.s_addr = INADDR_ANY;		/* Get my IP address */
	memset(&(server_addr.sin_zero), '\0', 8); 		/* Zero it out */
	
	/****** Try connecting to the server. *********/	
	ret_connect = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));	
	
	if(ret_connect == -1)
		perror("connect");

	int x;
	x=fcntl(sockfd,F_GETFL,0);
	fcntl(sockfd,F_SETFL,x | O_NONBLOCK);

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
			
			if ((recv_bytes = recv(sockfd, (void *)msg, sizeof(msg), 0)) <= 0) 
			{
				if (recv_bytes == 0) 
				{
					printf("Connection closed\n");
				}
				else 
				{
					perror("recv");
				}
				close(sockfd);
				FD_CLR(sockfd, &master);
			}
			else 
			{
				msg[recv_bytes] = '\0';
				printf("\nReceived: %s\n",msg);

				
				if (strstr(msg, HeaderValid) != NULL)
				{
					username_btn->disable();
					password_btn->disable();
				}
				else
				{
					if (strstr(msg, HeaderInvalid) != NULL)
					{
						textDisplay("Invalid User");
					}
					else
					{
						if (strstr(msg, HeaderAttendance) != NULL)
						{
							char att[10];
							strcpy(att, " ");
							for (k = 4; k < strlen(msg); k++)
							{
								att[k-4] = msg[k];
							}
							printf("attendance: %s",att);
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
						}

					}
				}
			}	
		}		
	}
	else
	{
		
	}
	
	
}



/**************************************** control_cb() *******************/
/* GLUI control callback                                                 */

void control_cb( GLUI_Control* control )
{	
	switch (control->get_id())
	{
		case connectToServer:
			connectTo();
			break;
			
		case disconnectFromServer:
			/******************* Release all resources ********************/
			//printf("Connection closed from %s\n",inet_ntoa(server_addr.sin_addr));
			textDisplay("Connection closed");
			close(sockfd);
			break;
			
		case capScreen_capture:
			break;
		case message_create:
		{
			messageWindow = GLUI_Master.create_glui( "Enter Message:",0, 80, 400 );
			
			GLUI_Panel *panel = new GLUI_Panel(messageWindow,"", GLUI_PANEL_NONE);
			message = new GLUI_TextBox(panel,true);
			message->set_text("");
			message->set_h(300);
			message->set_w(310);
			message->enable();
			
			new GLUI_Button(panel, "Send", message_sendto, message_cb);
			new GLUI_Button(panel, "Close", message_close, message_cb);
			
			messageWindow->set_main_gfx_window( main_window );
		}
			break;

		case fileUpload:
		{
			fileBrowser = GLUI_Master.create_glui( "List of files:",0, 80, 300 );
			
			fb = new GLUI_FileBrowser(fileBrowser, "", false, FBW, fileBrowse_cb);
			fileName = new GLUI_CommandLine(fileBrowser, "FileName:", NULL, -1, fileBrowse_cb);
			fileName->set_w(130); 
			
			GLUI_Panel *panel = new GLUI_Panel(fileBrowser,"", GLUI_PANEL_NONE);
			new GLUI_Button(panel, "Send", FBW_upload, fileBrowse_cb);
			new GLUI_Column(panel, false);
			new GLUI_Button(panel, "Close", FBW_close, fileBrowse_cb);
			
			fileBrowser->set_main_gfx_window( main_window );
			
			
		}
			break;
			
		case fileDownload:
			
			break;

		case attendance:
			strcpy(msg, HeaderAttendance);			
			send_bytes = send(sockfd, (void *)msg, strlen(msg), 0);
			
			if(send_bytes != -1) {
				// printf("\n%s\n", msg);
				textDisplay("Request for attendance is sent");
			} else {
				perror("send");
				exit(1);
			}
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
	
	/*  GLUI_Master.sync_live_all();  ** not needed * nothing to sync in this application  */
	
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
	textDisplay("This is the app for student");
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
	
	main_window = glutCreateWindow( "iLab Student" );
	glutDisplayFunc( myGlutDisplay );
	GLUI_Master.set_glutReshapeFunc( myGlutReshape );  
	GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
	GLUI_Master.set_glutSpecialFunc( NULL );
	GLUI_Master.set_glutMouseFunc( myGlutMouse );
	
	
	/*** Create the side subwindow ***/
	glui = GLUI_Master.create_glui_subwindow( main_window, GLUI_SUBWINDOW_RIGHT );
	
	
	new GLUI_Button( glui, "Connect", connectToServer, control_cb);
	
	
	/****************************************/
	/*   Login Panel init and adding objs   */
	/****************************************/	
	login_panel = new GLUI_Rollout(glui, "Login", false );
	username_btn = new GLUI_Button(login_panel, "Username", login_username, login_cb);
	password_btn = new GLUI_Button(login_panel, "Password", login_password, login_cb);
	
	
	/*********************************************/
	/* Capture screen Panel init and adding objs */
	/*********************************************/	
	//capScreen_panel = new GLUI_Rollout(glui, "Capture Screen", false );
	//new GLUI_Button(capScreen_panel, "Capture Screen", capScreen_capture, control_cb);
	
	
	/**************************************/
	/* Message Panel init and adding objs */
	/**************************************/	
	message_panel = new GLUI_Rollout(glui, "Message", false );
	new GLUI_Button(message_panel, "Create", message_create, control_cb);	
	
	/**************************************/
	/* Evaluation Panel init and adding objs */
	/**************************************/	
	evaluation_panel = new GLUI_Rollout(glui, "File transfer", false );
	new GLUI_Button(evaluation_panel, "Upload", fileUpload, control_cb);
	new GLUI_Button(evaluation_panel, "Download", fileDownload, control_cb);
	
	
	
	new GLUI_Button( glui, "Disconnect", disconnectFromServer,control_cb);
	attendance_btn = new GLUI_Button( glui, "Attendance", attendance,control_cb);
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




