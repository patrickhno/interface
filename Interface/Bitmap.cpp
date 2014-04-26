
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <memory.h>
#include "Bitmap.h"
#include "Rect.h"
#include "ARGB.h"
#include <posix/stdlib.h>
//#include <stdio.h>
//#define INTERNAL_DEFINES
#include <posix/stdio.h>
//#undef INTERNAL_DEFINES
//#include <posix/stdio.h>

#include <Interface/DynamicArray.h>
#include <Interface/String.h>
#include <assert.h>

static DynamicArray *BitmapCache = 0;
struct BmCache{
	String		Name;
	Bitmap		*Bitmap;
};

Bitmap::Bitmap(const String &name, const int _format) : BitmapLoader(), BitmapDriver(){
//printf("%s\n",name);

	// Filename caching
	Cache = 0;
	CacheCount = 0;

if(!BitmapCache) BitmapCache = new DynamicArray();

	int cnt = BitmapCache->GetCount();
	for(int n=0; n<cnt; n++){
		BmCache *c = (BmCache*)BitmapCache->GetItem(n);
		if(c->Name==name){
			c->Bitmap->AddCache();
			Cache = c->Bitmap;
			Width = Cache->Width;
			Height = Cache->Height;
//			Frames = 0;
			frame = 0;
BitmapDriver::SetOwner(this);
			return;
		}
	}

	BmCache *c = new BmCache;
	c->Name = name;
	c->Bitmap = this;
	BitmapCache->Add(c);

	bool inv = OpenBitmapFile((char*)name.GetString());
	Width  = GetBitmapWidth();
	Height = GetBitmapHeight();
	_Bitmap = 0;
	isfile=true;
	format=_format;
	translusency=0;
	framebuf = -1;
	frame=0;
	count = 0;
	BitmapDriver::SetOwner(this);
//	if(inv) Invalidate();
//printf("%d,%d\n",Width,Height);
}
Bitmap::Bitmap(class Rect &rect, int _format) : BitmapLoader(), BitmapDriver(){

	Cache = 0;
	CacheCount = 0;

	isfile=false;
	Width = int(rect.Width);
	Height = int(rect.Height);
	_Bitmap = (ARGB*)malloc(sizeof(ARGB)*Width*Height);
	format=_format;
	translusency=0;
	framebuf = -1;
	frame=0;
	count = 0;
	BitmapDriver::SetOwner(this);
//printf("%d,%d\n",Width,Height);
//	Invalidate();
}

Bitmap::Bitmap(Bitmap *bm) : BitmapLoader(), BitmapDriver(){

	Cache = 0;
	CacheCount = 0;

	isfile	= false; //bm->isfile;
	Width	= bm->GetWidth();
	Height	= bm->GetHeight();
	_Bitmap	= (ARGB*)malloc(sizeof(ARGB)*Width*Height);
	format=bm->format;
	translusency=0;
	framebuf = -1;
	frame=0;
	count = 0;
void *buf = bm->LoadBuffer(0);
	memcpy(_Bitmap,buf,sizeof(ARGB)*Width*Height);
bm->UnloadBuffer();
	BitmapDriver::SetOwner(this);
//for(int n=0; n<Width*Height; n++) _Bitmap[n]=ARGB(0,0,0);
//printf("%d,%d\n",Width,Height);
//	Invalidate();
}

//Bitmap::Bitmap(Bitmap *bm,bool,bool) : BitmapLoader(), BitmapDriver(){
//	isfile	= false; //bm->isfile;
//	Width	= bm->GetWidth();
//	Height	= bm->GetHeight();
//	_Bitmap	= (ARGB*)malloc(sizeof(ARGB)*Width*Height);
//	format=bm->format;
//	translusency=0;
//	framebuf = -1;
//	frame=0;
//	count = 0;
//void *buf = bm->LoadBuffer(0);
//	memcpy(_Bitmap,buf,sizeof(ARGB)*Width*Height);
//bm->UnloadBuffer();
//	BitmapDriver::SetOwner(this);
//for(int n=0; n<Width*Height; n++) _Bitmap[n]=ARGB(0,0,0);
//printf("%d,%d\n",Width,Height);
//	Invalidate();
//}

/*
Bitmap::Bitmap(Bitmap &bm) : BitmapLoader(), BitmapDriver(){
	bool inv = OpenBitmapFile(bm);
	Width  = bm.Width;
	Height = bm.Height;
	_Bitmap = 0;
	isfile=true;
	format=_format;
	translusency=0;
	BitmapDriver::SetOwner(this);
	if(inv) Invalidate();
}
*/
Bitmap::~Bitmap(){

assert(CacheCount==0);
	if(!Cache){
		if(isfile){ if(_Bitmap) UnloadFrame(_Bitmap);}
		else if(_Bitmap) free(_Bitmap);
	}

	if(BitmapCache){
		int cnt = BitmapCache->GetCount();
		for(int n=0; n<cnt; n++){
			BmCache *c = (BmCache*)BitmapCache->GetItem(n);
			if(c->Bitmap==this){
				BitmapCache->Remove(n);
				delete c;
				break;
			}
		}
	}
	if(Cache) Cache->SubCache();
}

void Bitmap::Clear(void){

	assert(Cache==0);

	//ARGB col = ARGB(0,0,0,0);
	for(int n=0; n<Width*Height; n++){
		_Bitmap[n].A=0;
		_Bitmap[n].R=0;
		_Bitmap[n].G=0;
		_Bitmap[n].B=0;
	}
//	Invalidate();
}

//#include "Process.h"

class ARGB *Bitmap::LoadBuffer(int _frame){

//	assert(Cache==0);
//printf("A\n");
	if(Cache) return Cache->LoadBuffer(_frame);
	else{
//printf("B\n");

//printf("Bitmap::LoadBuffer\n");
//printf("Loading frame %d\n",_frame);
		if(isfile){
//printf("B.1\n");
			if(_Bitmap){
//printf("B.2\n");
				if(framebuf==_frame){
//printf("B.3\n");
					count++;
//printf("B.4\n");
					return _Bitmap;
				}
//printf("B.5\n");
				UnloadFrame(_Bitmap);
//printf("B.6\n");
			}
//printf("C\n");
			_Bitmap = LoadFrame(_frame);
//printf("D\n");
			Width  = GetBitmapWidth();
//printf("E\n");
			Height = GetBitmapHeight();
//printf("F\n");
			framebuf = _frame;
			count++;
		}
		return _Bitmap;
	}
}

void Bitmap::UnloadBuffer(void){

//	assert(Cache==0);
	if(Cache) Cache->UnloadBuffer();
	else{
		if(isfile){
			count--;
			if(!count){
				//if(_Bitmap) UnloadFrame(_Bitmap);
				//_Bitmap=0;
				//framebuf = -1;
			}
		}
	}
}

int Bitmap::GetWidth(void){
	//assert(Cache==0);
	if(Cache) return Cache->GetWidth();
	else return(Width);
}

int Bitmap::GetHeight(void){
	//assert(Cache==0);
	if(Cache) return Cache->GetHeight();
	else return(Height);
}

int Bitmap::GetFrameCount(void){
	//assert(Cache==0);
	if(Cache) return Cache->GetFrameCount();
	else{
		if(isfile) return(BitmapLoader::GetFrameCount());
		else return(1);
	}
}

void Bitmap::CopyRect(Bitmap *frombm, const Rect &from, const Rect &to){
	assert(Cache==0);
	ARGB *fbm = frombm->LoadBuffer(0);
	int tox   = int(to.x);
	int toy   = int(to.y);
	int toly  = int(to.ly);
	int torx  = int(to.rx);
	int fromx = int(from.x);
	int fromy = int(from.y);
	int fh_over_th = int((from.Height/to.Height)*256.0f);
	int fw_over_tw = int((from.Width/to.Width)*256.0f);
	for(int y=toy; y<toly; y++){
		for(int x=tox; x<torx; x++){
			_Bitmap[y*Width+x]=
				fbm[(fromy+((y-toy)*fh_over_th)>>8)*frombm->GetWidth()+
					 fromx+((x-tox)*fw_over_tw)>>8];
		}
	}
	frombm->UnloadBuffer();
//	Invalidate();
}

void Bitmap::Render(class View *view){
	assert(Cache==0);
	BitmapDriver::Render(view,translusency);
}

Bitmap &Bitmap::operator*=(ARGB &col){
	assert(Cache==0);
	for(int y=0; y<Height; y++){
		for(int x=0; x<Width; x++){
			_Bitmap[y*Width+x]*=col;
		}
	}
//	Invalidate();
	return *this;
}

Bitmap &Bitmap::operator|=(ARGB &col){
	assert(Cache==0);
	for(int y=0; y<Height; y++){
		for(int x=0; x<Width; x++){
			_Bitmap[y*Width+x]|=col;
		}
	}
//	Invalidate();
	return *this;
}

void Bitmap::SetAlpha(float val){
	assert(Cache==0);
	for(int y=0; y<Height; y++){
		for(int x=0; x<Width; x++){
			_Bitmap[y*Width+x].A=val;
		}
	}
//	Invalidate();
}

void Bitmap::CalculateAlpha(void){
	assert(Cache==0);
	for(int y=0; y<Height; y++){
		for(int x=0; x<Width; x++){
			_Bitmap[y*Width+x].A=((float)y)/Height; //(_Bitmap[y*Width+x].R+_Bitmap[y*Width+x].G+_Bitmap[y*Width+x].B)/3.0f;
		}
	}
//	Invalidate();
}

int Bitmap::GetFormat(void){
//	assert(Cache==0);
	if(Cache) return Cache->GetFormat();
	else return(format);
}

void Bitmap::SetTranslusency(float val){
	assert(Cache==0);
	translusency = val;
}

void Bitmap::Flush(void){
	assert(Cache==0);
	Invalidate();
}

void Bitmap::SetFrame(int f){
	assert(Cache==0);
	frame = f;
//	Invalidate();
}

int Bitmap::GetFrame(void) const{
	assert(Cache==0);
	return frame;
}
/*
//#include <posix/va_list.h>
#include <stdarg.h>

void Bitmap::Print(const Font *font, const char *fmt, ...){
	char str[1024];
	va_list args;
	va_start(args,fmt);
	vsprintf(str,fmt,args);
	char *s = str;
	do{
		char c=*s++;
		if(!c) break;
	}while(true);
}
*/

void Bitmap::Add(Bitmap *from){
	assert(Cache==0);
	if(isfile){
		LoadBuffer(0);
		isfile = false;
		count=0;
	}
	ARGB *fbm = from->LoadBuffer(0);
	int w = GetWidth();
	int h = GetHeight();
	int fw = from->GetWidth();
	int fh = from->GetHeight();
	for(int y=0; y<h; y++){
		for(int x=0; x<w; x++){
//printf("%d,%d,%d,%d,%d,%d\n",x,y,w,h,fw,fh);
			if(x<fw&&y<fh){
				ARGB col = _Bitmap[y*Width+x]+fbm[y*fw+x];
				if(col.A>1.0f) col.A = 1.0f;
				if(col.R>1.0f) col.R = 1.0f;
				if(col.G>1.0f) col.G = 1.0f;
				if(col.B>1.0f) col.B = 1.0f;
			}
		}
	}
	from->UnloadBuffer();
//	Invalidate();
}

void Bitmap::Scale(const Rect &rect){
	assert(Cache==0);
	ARGB *obm = _Bitmap;
	int nw = int(rect.Width);
	int nh = int(rect.Height);
	_Bitmap = (ARGB*)malloc(nw*nh*sizeof(ARGB));
	int w = nw;//GetWidth()  > nw ? nw : GetWidth();
	int h = nh;//GetHeight() > nh ? nh : GetHeight();
	int sx = int((float(GetWidth())/rect.Width)*255.0f);
	int sy = int((float(GetHeight())/rect.Height)*255.0f)*GetWidth();
	for(int y=0; y<h; y++){
		for(int x=0; x<w; x++) _Bitmap[y*nw+x]=obm[((y*sy)>>8)+((x*sx)>>8)];
	}
	Width  = nw;
	Height = nh;
	free(obm);
}

