/*
 *  headers.h
 *  iLabServer
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
 1020 - file
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

#include <iostream.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sstream>
#include <fstream.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>


#define WINDOW_HEIGHT		 glutGet(GLUT_SCREEN_HEIGHT)
#define WINDOW_WIDTH		 glutGet(GLUT_SCREEN_WIDTH)
#define connectServer		  10
#define disconnectServer	  11
#define login_attendance	 100
#define login_password		 101
#define capScreen_student	 200
#define capScreen_radio		 201
#define capScreen_capture	 202
#define message_create		 300
#define message_sendto		 301
#define message_sendtoAll	 302
#define message_restart		 303
#define message_shutdown	 304
#define evaluate_student	 400
#define evaluate			 40
#define showAttendance		500
#define showLogin			501
#define closeLabel			502

#define MESSAGE_SIZE		 INT_MAX
#define SERVER_PORT			 6549
#define BACKLOG				 30

char HeaderUsername[5] = "1000";
char HeaderPassword[5] = "1001";
char HeaderValid[5] = "1002";
char HeaderInvalid[5] = "1003";
char HeaderMessage[5] = "1010";
char HeaderAttendance[5] = "1040";
char HeaderFileName[5] = "1020";
char HeaderFileUpload[5] = "1021";
char HeaderFileDownload[5] = "1022";
