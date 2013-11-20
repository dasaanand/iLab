/*
 *  headers.h
 *  iLabClient
 *
 *  Created by Dasa Anand on 19/02/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


/* 
 1000 - username
	1001 - password
	1002 - valid user
	1003 - invalid user
 1010 - message
 1020 - fileName
	1021 - file upload
	1022 - file download
 1030 - change pswd
 1040 - check attendance
 */



#ifdef __APPLE__ 
#include <GLUT/GLUT.h>
#include <GLUI/GLUI.h>
#include <OpenGL/OpenGL.h>
#include <alloca.h>

#else
#include <gl/glut.h>
#include <gl/glui.h>
#include <malloc.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>


#define WINDOW_HEIGHT		 glutGet(GLUT_SCREEN_HEIGHT)
#define WINDOW_WIDTH		 glutGet(GLUT_SCREEN_WIDTH)
#define connectToServer		  10
#define disconnectFromServer  11
#define login_username		 100
#define login_password		 101
#define capScreen_capture	 200
#define message_create		 300
#define fileUpload			 400
#define fileDownload		 401
#define attendance			 500

#define loginW_username		900
#define loginW_password		901
#define close_label			1000
#define message_sendto		902
#define message_close		903
#define FBW_upload			904
#define FBW_close			905
#define FBW					906

#define MESSAGE_SIZE		 INT_MAX
#define SERVER_PORT			 6549


char HeaderUsername[5] = "1000";
char HeaderPassword[5] = "1001";
char HeaderValid[5] = "1002";
char HeaderInvalid[5] = "1003";
char HeaderMessage[5] = "1010";
char HeaderAttendance[5] = "1040";
char HeaderFileName[5] = "1020";
char HeaderFileUpload[5] = "1021";
char HeaderFileDownload[5] = "1022";

