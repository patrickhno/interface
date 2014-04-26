
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <stdarg.h>
#include "View.h"
#include "DynamicArray.h"
#include <3dapi/Camera.h>
#include <3dapi/Network/Host.h>
#include <Interface/Bitmap.h>
#include <posix/stdlib.h>
#include <3dapi/Geometry.h>
#include <3dapi/LightPoint.h>
#include <Interface/Bitmap32.h>
#include <assert.h>

#ifdef OPENGL

View::View(const Rect &r){
	rect = r;
	bitmaps32 = new DynamicArray();
	views     = new DynamicArray();
	parent = 0;
}
View::~View(){
	if(parent) parent->Detatch(this);
	assert(bitmaps32->GetCount()==0);
	assert(views->GetCount()==0);
	delete bitmaps32;
	delete views;
}
void View::SetParent(View *p){
	parent = p;
}
void View::Set(const class Rect &r){
	rect = r;
}
void View::SetClearState(const bool state){
	clear = state;
}
void View::SetClip(const bool state){
	clip = state;
}
void View::SetColor(const class ARGB &col){
	color = col;
}
Font *View::*GetFont(){
	return font;
}
void View::Apply(const class Font *f){
	font = f;
}
void View::Apply(const class Bitmap32 *bm){
	bitmaps32->Add(bm);
}
void View::Apply(const class View *v){
	v->SetParent(this);
	views->Add(v);
}
void View::Detatch(const class Bitmap32 *bm){
	int cnt = bitmaps32->GetCount();
	for(int n=0; n<cnt; n++){
		if((Bitmap32*)bitmaps32->GetItem(n)==bm){
			bitmaps32->Remove(n);
			return;
		}
	}
}
void View::Detatch(const class View *v){
	int cnt = views->GetCount();
	for(int n=0; n<cnt; n++){
		if((Bitmap32*)views->GetItem(n)==bm){
			views->Remove(n);
			return;
		}
	}
}
Rect View::*GetAbsolute(){
	return(&Absolute);
}
void View::Render(){
	glPushMatrix();
	glTranslatef(0,0,-4);
	glRotatef(50,0,1,0);
	glPopMatrix();
}

#else

// Public

View::View(Rect &rect) : ConsoleStream(rect){
	Screen = 0;
	Parent = 0;
	_ConsoleStream_SetView(this);	// Drivers need this
	ViewRect=rect;
	ChildViews	= new DynamicArray(DARRAY_SHARED_MEMORY);	// fixme: First view may be in another process.		//DARRAY_GENID);	// fixme: GENID?
	Bitmaps   = new DynamicArray(DARRAY_GENID);		// fixme: GENID?
	Bitmaps32 = new DynamicArray(DARRAY_GENID);		// fixme: GENID?
	Color=ARGB(0,0,0,0);
	DummyCamera = 0; ////new Camera();
	////Attatch(DummyCamera);
ViewCamera = 0;
	extern Kernel *__global__Kernel__;
	Host *host=__global__Kernel__->GetHost();
//	KeyDownHook=host->GetHook("+key");
//	KeyUpHook=host->GetHook("-key");
	extern Process *ThisProcess;
	CreatorProcess = ThisProcess;
	DetatchViews = new DynamicArray(DARRAY_SHARED_MEMORY);
	collisions = true;

	transparency = ARGB(0,0,0,0);
	clip = true;
}

View::~View(){
	delete DetatchViews;
	delete Bitmaps;
	delete Bitmaps32;
	delete ChildViews;
	////delete DummyCamera;
	if(Parent) Parent->Detatch(this);
// fixme: remove lightpoints from native!
}

void View::Set(Rect &rect){
	ViewRect = rect;
	if(Parent){
		SetAbsolute(Rect(
			Parent->GetAbsolute()->x+Parent->GetAbsolute()->Width*ViewRect.x,  Parent->GetAbsolute()->y+Parent->GetAbsolute()->Height*ViewRect.y,
			Parent->GetAbsolute()->x+Parent->GetAbsolute()->Width*ViewRect.rx, Parent->GetAbsolute()->y+Parent->GetAbsolute()->Height*ViewRect.ly
		));
	}
}

void View::Detatch(View *view){
//printf("View::Detatch: %x\n",this);
//printf("View::Detatch: %x\n",DetatchViews);
//printf("View::Detatch: %x\n",view);
	#ifdef _DEBUG
	DetatchViews->Add(view,__FILE__,__LINE__);
	#else
	DetatchViews->Add(view);
	#endif
}

int View::Apply(View *view){
	view->SetScreen(Screen);
	view->SetAbsolute(Rect(
		Absolute.x+Absolute.Width*view->ViewRect.x, Absolute.y+Absolute.Height*view->ViewRect.y,
		Absolute.x+Absolute.Width*view->ViewRect.rx, Absolute.y+Absolute.Height*view->ViewRect.ly
	));
	view->SetParent(this);
	#ifdef _DEBUG
	return ChildViews->Add(view,__FILE__,__LINE__);
	#else
	return ChildViews->Add(view);
	#endif
}

int View::Apply(Bitmap *bm){
	#ifdef _DEBUG
	return Bitmaps->Add(bm,__FILE__,__LINE__);
	#else
	return Bitmaps->Add(bm);
	#endif
}

int View::Apply(Bitmap32 *bm){
//printf("%x\n",this);
//printf("a: %x,%x\n",this,bm);
//printf("!!%d\n",Bitmaps32->GetCount());
	#ifdef _DEBUG
	return Bitmaps32->Add(bm,__FILE__,__LINE__);
	#else
	return Bitmaps32->Add(bm);
	#endif
}

void View::Detatch(Bitmap32 *bm){
//printf("D: %x,%x\n",this,bm);
	int cnt = Bitmaps32->GetCount();
//printf("!!%d\n",cnt);
	for(int n=0; n<cnt; n++){
//printf("%d: %x\n",n,Bitmaps32->GetItem(n));
		if(Bitmaps32->GetItem(n)==(void*)bm){
			Bitmaps32->Remove(n);
			return;
		}
	}
	assert(false);
}

void View::Apply(Geometry *geo){
////	ViewCamera->Apply(geo);
	ViewCamera->Apply(0,geo->GetNode());
}

//void View::Apply(class LightPoint *lp){			// fixme: this stuff shouldnt be here
//	extern Kernel *__global__Kernel__;
//	__global__Kernel__->AddLightPoint(lp);		// Native keeps track of things
////	return ViewCamera->Apply(lp);
//	ViewCamera->Apply(0,lp->GetNode());
//}

void View::SetColor(ARGB &col){
	Color=col;
}

Rect *View::GetRect(void){
	return(&ViewRect);
}

Rect *View::GetAbsolute(void){
	return(&Absolute);
}

Rect *View::GetAbsoluteBorder(void){
	Border = Absolute;
	if(Parent){
		Rect *p = Parent->GetAbsoluteBorder();
		if(clip){
			if(Border.y <p->y ) Border.y  = p->y;
			if(Border.ly>p->ly) Border.ly = p->ly;
			if(Border.x <p->x ) Border.x  = p->x;
			if(Border.rx>p->rx) Border.rx = p->rx;
			Border.Width  = Border.rx - Border.x;
			Border.Height = Border.ly - Border.y;
		}else{
			Border.y  = p->y;
			Border.ly = p->ly;
			Border.x  = p->x;
			Border.rx = p->rx;
			Border.Width  = p->Width;
			Border.Height = p->Height;
		}
	}
	return(&Border);
}

ARGB *View::GetColor(void){
	return(&Color);
}

void View::Attatch(Camera *cam){
	ViewCamera = cam;
}

class WindowScreen *View::GetScreen(void){
	return(Screen);
}

// Hooks

void View::MessageReceived(double time, int Cmd, void *){}
void View::KeyDown(double time, uint8 Key){}
void View::KeyUp(double time, uint8 Key){}

// Private

void View::_view_MessageReceived(double time, int Cmd, void *args){
	MessageReceived(time,Cmd,args);
}

void View::_view_Invalidate(void){
	ViewDriver::Invalidate();
	// Set child views
	int cnt=ChildViews->GetCount();
	for(int n=0; n<cnt; n++){
		View *view=(View*)ChildViews->GetItem(n);
		view->_view_Invalidate();
	}
}

void View::SetScreen(class WindowScreen *scr){
	Screen=scr;
	// Set child views
	int cnt=ChildViews->GetCount();
	for(int n=0; n<cnt; n++){
		View *view=(View*)ChildViews->GetItem(n);
		view->SetScreen(scr);
	}
}

bool View::RefreshView(int pass, double time, class WindowScreen *Screen){
//printf("RefreshView:a\n");
	if(pass==0){
		done=false;
//printf("RefreshView:b\n");
		ViewDriver::RenderView(Screen);	// Clears view
//printf("RefreshView:c\n");
		if(ViewCamera) ViewCamera->Tick(time);			// Calculate AI, calclulate rotation quaternions etc.
//printf("RefreshView:d\n");

		// Detatch views
		while(DetatchViews->GetCount()){
//printf("RefreshView:e\n");
			View *view = (View*)DetatchViews->GetItem(0);
//printf("RefreshView:f\n");
			#ifdef _DEBUG
			DetatchViews->Remove(0,__FILE__,__LINE__);
			#else
			DetatchViews->Remove(0);
			#endif
//printf("RefreshView:g\n");
			int cnt=ChildViews->GetCount();
			for(int n=0; n<cnt; n++){
				if(view==ChildViews->GetItem(n)){
//printf("RefreshView:h\n");
					#ifdef _DEBUG
					ChildViews->Remove(n,__FILE__,__LINE__);
					#else
					ChildViews->Remove(n);
					#endif
					break;
//printf("RefreshView:i\n");
				}
			}
		}
//printf("RefreshView:j\n");

		//int n=10;
		#if 1
////		if(collisions) while(n--&&ViewCamera->CollideCheck());
		if(ViewCamera) ViewCamera->CollideCheck();
		//if(n<0) printf("%d Collide check overflow!\n",n);
//printf("RefreshView:k\n");
		#endif
		// Render bitmaps
		int cnt = Bitmaps->GetCount();
		for(int n=0; n<cnt; n++){
			Bitmap *bm = (Bitmap*)Bitmaps->GetItem(n);
			bm->Render(this);
		}
//printf("RefreshView:l\n");
/*
		cnt = Bitmaps32->GetCount();
//printf("!!! %x,%d\n",this,cnt);
		for(n=0; n<cnt; n++){
			Bitmap32 *bm = (Bitmap32*)Bitmaps32->GetItem(n);
			bm->Render(this);
		}
*/
		cnt = Bitmaps32->GetCount();
		for(n=0; n<cnt; n++){
			Bitmap32 *bm = (Bitmap32*)Bitmaps32->GetItem(n);
			bm->Prepare(this);
		}
//printf("RefreshView:m\n");
	}
	bool ret=false;
	if(done){
		ret=false;
		// Render child views
		int cnt=ChildViews->GetCount();
		for(int n=0; n<cnt; n++){
			int pass=0;
			while(((View*)ChildViews->GetItem(n))->RefreshView(pass,time,Screen)) pass++;
		}

		// Detatch views
		while(DetatchViews->GetCount()){
			View *view = (View*)DetatchViews->GetItem(0);
			#ifdef _DEBUG
			DetatchViews->Remove(0,__FILE__,__LINE__);
			#else
			DetatchViews->Remove(0);
			#endif
			int cnt=ChildViews->GetCount();
			for(int n=0; n<cnt; n++){
				if(view==ChildViews->GetItem(n)){
					#ifdef _DEBUG
					ChildViews->Remove(n,__FILE__,__LINE__);
					#else
					ChildViews->Remove(n);
					#endif
				}
			}
		}

	}else{
		/*
		ret = ViewCamera->Render(pass,this);		// Render 3D scene
		if(!ret){
			done=true;
			ret=true;
			_RenderConsole(Buffer);			// Render console text
		}
		*/

		// Render bitmaps
		int cnt = Bitmaps32->GetCount();
		for(int n=0; n<cnt; n++){
			Bitmap32 *bm = (Bitmap32*)Bitmaps32->GetItem(n);
			bm->Render(this,transparency);
		}

		if(ViewCamera) ViewCamera->_viewRender(this);
		done=true;
		ret=true;
		ConsoleStream::sem->Acquire();
		_RenderConsole(Buffer);			// Render console text
		ConsoleStream::sem->Release();
	}
	return ret;
}

void View::SetAbsolute(Rect &rect){
	Absolute=rect;
//printf("%f,%f,%f,%f\n",rect.x,rect.y,rect.rx,rect.ly);
	// Set child views
	int cnt=ChildViews->GetCount();
	for(int n=0; n<cnt; n++){
		View *view=(View*)ChildViews->GetItem(n);		// fixme: view is invalid on shutdown.
		view->SetAbsolute(Rect(
			Absolute.x+Absolute.Width*view->ViewRect.x,  Absolute.y+Absolute.Height*view->ViewRect.y,
			Absolute.x+Absolute.Width*view->ViewRect.rx, Absolute.y+Absolute.Height*view->ViewRect.ly
		));
	}
}

void View::Clear(class WindowScreen *Screen){
	ViewDriver::Clear(Screen);
}

void View::SetParent(View *p){
	Parent = p;
}

void View::DisableCollisions(void){	// fixme: This shouldnt be here!!
	collisions = false;
}

void View::SetTransparency(ARGB &tr){
	transparency = tr;
}

#endif
