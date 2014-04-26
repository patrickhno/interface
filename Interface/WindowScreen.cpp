
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <posix/stdlib.h>
#include <posix/stdio.h>
#include <stdarg.h>
#include "WindowScreen.h"

#ifdef OPENGL
/*
#include <GL/glut.h>

WindowScreen::WindowScreen(), View(Rect(0,0,1,1)){
	SetAbsolute(Rect(0,0,1,1));
	SetScreen(this);
	otime = -1;
	timeofs=-1;

	glutInitWindowPosition(0,0);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutInit(&ac,av);

	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);

	if(!(win=glutCreateWindow("R-OS"))){
		fprintf(stderr,"Error, couldn't open window\n");
		return -1;
	}

	reshape(WIDTH,HEIGHT);

	//glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glutReshapeFunc(gl_reshape);
	glutDisplayFunc(gl_draw);
	glutKeyboardFunc(gl_key);
	glutSpecialFunc(gl_special);
	glutIdleFunc(gl_draw);

//	glutMainLoop();
}
*/
#else

//enum{
//	CMD_REFRESH
//};

WindowScreen::WindowScreen() : WindowScreenDriver(), View(Rect(0,0,1,1)){ //, Message(){
	extern Process *ThisProcess;
//	if(ThisProcess==0){
//		SetName("Servers/Interface");
//	}
	SetAbsolute(Rect(0,0,1,1));
	SetScreen(this);
//	extern Kernel *__global__Kernel__;
//	__global__Kernel__->AddWindowScreen(this);	// Native keeps track of things
	otime = -1;
	timeofs=-1;
}

WindowScreen::~WindowScreen(){
	// fixme: Screen must be removed from native!
}

void WindowScreen::Refresh(void){
//	extern Process *ThisProcess;
//	if(ThisProcess){
		// Tell kernel process to refresh
//		AddCmd(CMD_REFRESH);
//		SendTo("Servers/Interface");
//	}else{
		double time = Time();
		if(timeofs<0) timeofs=time;
		double real = time;
		time-=timeofs;
		if(otime<0) otime = time;
		if(time-otime>30.0f) time-=(time-otime);
		timeofs=real-time;
		otime = time;
//printf("WindowScreen::a\n");
//printf("%f\n",time);
		int pass = 0;
		while(RefreshView(pass++,time,this));
//printf("WindowScreen::b\n");
		_ScreenUpdate();		// Waits for VBL, and swaps buffers
//printf("WindowScreen::c\n");
//	}
}

int WindowScreen::GetWidth(void){
	return(_GetWidth());
}

int WindowScreen::GetHeight(void){
	return(_GetHeight());
}

void WindowScreen::CycleMode(void){
	_view_Invalidate();
	_CycleMode();
}

uint32 *WindowScreen::Lock(void){
	return(_Lock());
}

void WindowScreen::Unlock(void){
	_Unlock();
}

void WindowScreen::SetScale(float val){
	WindowScreenDriver::SetScale(val);
}

/*
void WindowScreen::MessageReceived(void){
	int cnt = GetCount();
	for(int n=0; n<cnt; n++){
		switch(GetReceivedCommand(n)){
		case CMD_REFRESH:
			Refresh();
		break;
		}
	}
}
*/
// Private
/*
void WindowScreen::__MessageReceived(double time, int Cmd, void *args){
//	extern Kernel *__global__Kernel__;
//	if(Cmd==__global__Kernel__->KeyDownHook)	KeyDown(time,((uint8*)args)[0]);
//	if(Cmd==__global__Kernel__->KeyUpHook)		KeyUp(time,((uint8*)args)[0]);
	_view_MessageReceived(time,Cmd,args);
}
*/
#endif
