
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <posix/stdlib.h>
#include <posix/string.h>
#include <posix/stdio.h>
#include <stdarg.h>
#include "ConsoleStream.h"
#include "DynamicBuffer.h"
#include <Interface/Semaphore.h>

// Public

ConsoleStream::ConsoleStream(Rect &rect) : ViewDriver(rect){
	sem = new Semaphore();
	sem->Acquire();
	Buffer=new DynamicBuffer(1024,DBUF_SHARED);
	LineCount=0;
	font = 0;
	sem->Release();
}
ConsoleStream::~ConsoleStream(){
	sem->Acquire();
	delete Buffer;
	sem->Release();
	delete sem;
}

// Dump console to stdout
void ConsoleStream::Dump(void){
//	char *str = (char*)Buffer->GetBuffer();
//	int count=Buffer->GetCount();
//	for(int n=0; n<count; n++) putchar(*str++);
}

void ConsoleStream::PutChar(char c){
	sem->Acquire();
//printf("%x,%c\n",this,c);
//	if((c>='a')&&(c<='z')) c=c-'a'+'A';	// fixme
	if(c=='\n') LineCount++;
	else if(c=='\r') LineCount++;		// !!!
	#ifdef _DEBUG
	Buffer->Add(&c,sizeof(char),__FILE__,__LINE__);
	#else
	Buffer->Add(&c,sizeof(char));
	#endif

	int remcount=LineCount-500;
	if(remcount>0){
		#ifdef _DEBUG
		char *buf=(char*)Buffer->GetBuffer(__FILE__,__LINE__);
		#else
		char *buf=(char*)Buffer->GetBuffer();
		#endif
		for(int n=0; n<remcount; n++){
			while((*buf!='\r')&&(*buf!='\n')) buf++;
			buf++;
			LineCount--;
		}
		#ifdef _DEBUG
		buf-=(int)Buffer->GetBuffer(__FILE__,__LINE__);
		#else
		buf-=(int)Buffer->GetBuffer();
		#endif
		Buffer->Cut(0,(int)buf);
	}
	sem->Release();
//printf("ConsoleStream::PutChar:%x,%x,%x\n",this,Buffer,Buffer->GetCount());
}

int ConsoleStream::Print(const char *fmt,...){
	sem->Acquire();
	va_list	argp;
	va_start(argp,fmt);
//	return vPrint(fmt,argp);
//}

//int ConsoleStream::vPrint(char *fmt, va_list argp){
//vprintf(fmt,argp);
/*
	int ch;
	for(int cnt = 0;; ++fmt){
		while((ch=*fmt)&&ch!='%'){
			PutChar(ch);
			fmt++; cnt++;
		}
		if(!ch) return cnt;
		switch(*++fmt){
//		case 'e':
//		case 'E':
//		case 'f':
//		case 'g':
//		case 'G':
//			long double _ldouble = va_arg(argp, long double);
//		break;
		case 's':
			char *str = va_arg(argp,char*);
			while(*str){ PutChar(*str++); cnt++; }
		break;
		}
	}
*/
	char str[1024];
	int len = vsprintf(str,fmt,argp);
	for(int n=0; n<len; n++) PutChar(str[n]);
	sem->Release();
	return len;
}

void ConsoleStream::operator<<(char *str){
}
void ConsoleStream::operator<<(Vector &v){
}
void ConsoleStream::operator<<(Quaternion &q){
}
void ConsoleStream::operator<<(Matrix3x3 &v){
}

// Private

void ConsoleStream::_ConsoleStream_Invalidate(void){
	sem->Acquire();
	_ConsoleDriver_Invalidate();
	sem->Release();
}

void ConsoleStream::_ConsoleStream_SetView(class View *view){
	sem->Acquire();
	ViewDriver::SetView(view);	// Pass it on down to the drivers
	sem->Release();
}

void ConsoleStream::_Apply(Font *_font){
//printf("AA\n");
	sem->Acquire();
//printf("BB (this = %x\n)\n",this);
	font = _font;
//printf("CC.\n");
	_ConsoleDriver_Invalidate();
//printf("DD\n");
	sem->Release();
//printf("EE\n");
}

class Font *ConsoleStream::GetFont(void){
	return font;
}

void ConsoleStream::Clear(void){
	sem->Acquire();
	Buffer->New();
	LineCount=0;
	sem->Release();
}

char *ConsoleStream::GetString(void){
//printf("ConsoleStream::GetString:%x,%x,%x\n",this,Buffer,Buffer->GetCount());
	char *foo = new char[Buffer->GetCount()+1];		// fixme
	memcpy(foo,Buffer->GetBuffer(),Buffer->GetCount());
	foo[Buffer->GetCount()]=0;
	return foo;
}

