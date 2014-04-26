#if 0
/**************************************************************\
|      Copyright (C) 1997 RealityMakers Entertainment AS       |
|                     All rights reserved.                     |
\**************************************************************/
#include <windows.h>
#include <time.h>

//#include <stdlib.h>
#include "Drivers.h"
#include "null_Drivers.h"
#include "glide_Drivers.h"
#include "gl_Drivers.h"
#include "Native.h"
#include "DynamicArray.h"
#include "WindowScreen.h"
#include "Host.h"
#include "AudioDevice.h"
#include "qmix_QMixer.h"
#include "rme_Memory.h"
#include "./Process.h"
#include "Server.h"
#include "InputDevice.h"

#include "Debug.h"

void Application(int argc, char **argv);

// Global stuff (for now)
Driver *Drivers;
int ActiveDriver;
Native *__global__Native__;
Process *ThisProcess;
//-------------

class Keyboard : InputDevice{
public:			Keyboard();
				~Keyboard();
		int		GetDigitalPortCount(void);
//		int		GetDigitalPortType(int digitalport);
//virtual	void	DigitalTogle(bool,char *);
private:
friend class Native;
		void	WindowsKey(bool,uint8);
		char	Keys[256][16];
};

class	Host	*host;
		double	StartTime;

Native::Native(int argc, char **argv) : Message(){
	SetName("Kernel");
	ThisProcess = 0;
	StartTime=clock();
server = new Server();	// Local server
	host=new Host();
	KeyDownHook=host->GetHook("+key");
	KeyUpHook=host->GetHook("-key");
	ScreenArray = new DynamicArray(DARRAY_GENID);		// fixme: GENID?
	LightPointArray = new DynamicArray(DARRAY_GENID);	// fixme: GENID?
	MaterialArray = new DynamicArray(DARRAY_GENID);		// fixme: GENID?
	InputDevices = new DynamicArray(DARRAY_GENID);		// fixme: GENID?
	__global__Native__=this;
//	Drivers=(Driver*)malloc(sizeof(struct Driver)*3);
	Drivers=(Driver*)Allocate(sizeof(struct Driver)*3,MEM_SHARED_READ);
	// null driver to help porting efforts
	Drivers[0].Initialize							= null_Initialize;
	Drivers[0].Deinitialize							= null_Deinitialize;
	Drivers[0].WindowScreen_Initialize				= null_WindowScreen_Initialize;
	Drivers[0].WindowScreen_Deinitialize			= null_WindowScreen_Deinitialize;
	Drivers[0].WindowScreen_Update					= null_WindowScreen_Update;
	Drivers[0].WindowScreen_GetWidth				= null_WindowScreen_GetWidth;
	Drivers[0].WindowScreen_GetHeight				= null_WindowScreen_GetHeight;
	Drivers[0].WindowScreen_CycleMode				= null_WindowScreen_CycleMode;
	Drivers[0].WindowScreen_Lock					= null_WindowScreen_Lock;
	Drivers[0].WindowScreen_Unlock					= null_WindowScreen_Unlock;
	Drivers[0].View_Initialize						= null_View_Initialize;
	Drivers[0].View_Deinitialize					= null_View_Deinitialize;
	Drivers[0].View_Invalidate						= null_View_Invalidate;
	Drivers[0].View_Update							= null_View_Update;
	Drivers[0].Geometry_Initialize					= null_Geometry_Initialize;
	Drivers[0].Geometry_Deinitialize				= null_Geometry_Deinitialize;
	Drivers[0].Geometry_Invalidate					= null_Geometry_Invalidate;
	Drivers[0].Geometry_Update						= null_Geometry_Update;
	Drivers[0].Geometry_SetVector					= null_Geometry_SetVector;
	Drivers[0].Camera_Initialize					= null_Camera_Initialize;
	Drivers[0].Camera_Deinitialize					= null_Camera_Deinitialize;
	Drivers[0].Camera_Invalidate					= null_Camera_Invalidate;
	Drivers[0].Camera_Render						= null_Camera_Render;
	Drivers[0].Camera_SetFieldOfView				= null_Camera_SetFieldOfView;
	Drivers[0].Console_Initialize					= null_Console_Initialize;
	Drivers[0].Console_Deinitialize					= null_Console_Deinitialize;
	Drivers[0].Console_Invalidate					= null_Console_Invalidate;
	Drivers[0].Console_Render						= null_Console_Render;
	Drivers[0].LightPoint_Initialize				= null_LightPoint_Initialize;
	Drivers[0].LightPoint_Deinitialize				= null_LightPoint_Deinitialize;
	Drivers[0].LightPoint_SetConstantAttenuation	= null_LightPoint_SetConstantAttenuation;
	Drivers[0].LightPoint_SetLinearAttenuation		= null_LightPoint_SetLinearAttenuation;
	Drivers[0].LightPoint_SetQuadraticAttenuation	= null_LightPoint_SetQuadraticAttenuation;
	Drivers[0].LightPoint_SetAmbientIntensity		= null_LightPoint_SetAmbientIntensity;
	Drivers[0].LightPoint_SetDiffuseIntensity		= null_LightPoint_SetAmbientIntensity;
	Drivers[0].LightPoint_SetSpecularIntensity		= null_LightPoint_SetAmbientIntensity;
	Drivers[0].IndexedPolygon_Initialize			= null_IndexedPolygon_Initialize;
	Drivers[0].IndexedPolygon_Deinitialize			= null_IndexedPolygon_Deinitialize;
	Drivers[0].IndexedPolygon_CachePolygon			= null_IndexedPolygon_CachePolygon;
	Drivers[0].IndexedPolygon_SetIteration			= null_IndexedPolygon_SetIteration;
	Drivers[0].Texture_Initialize					= null_Texture_Initialize;
	Drivers[0].Texture_Deinitialize					= null_Texture_Deinitialize;
	Drivers[0].Texture_SetFrame						= null_Texture_SetFrame;
	Drivers[0].Bitmap_Initialize					= null_Bitmap_Initialize;
	Drivers[0].Bitmap_Deinitialize					= null_Bitmap_Deinitialize;
	Drivers[0].Bitmap_Invalidate					= null_Bitmap_Invalidate;
	Drivers[0].Bitmap_Render						= null_Bitmap_Render;
	// Glide driver
	Drivers[1].Initialize							= glide_Initialize;
	Drivers[1].Deinitialize							= glide_Deinitialize;
	Drivers[1].WindowScreen_Initialize				= glide_WindowScreen_Initialize;
	Drivers[1].WindowScreen_Deinitialize			= glide_WindowScreen_Deinitialize;
	Drivers[1].WindowScreen_Update					= glide_WindowScreen_Update;
	Drivers[1].WindowScreen_GetWidth				= glide_WindowScreen_GetWidth;
	Drivers[1].WindowScreen_GetHeight				= glide_WindowScreen_GetHeight;
	Drivers[1].WindowScreen_CycleMode				= glide_WindowScreen_CycleMode;
	Drivers[1].WindowScreen_Lock					= glide_WindowScreen_Lock;
	Drivers[1].WindowScreen_Unlock					= glide_WindowScreen_Unlock;
	Drivers[1].View_Initialize						= glide_View_Initialize;
	Drivers[1].View_Deinitialize					= glide_View_Deinitialize;
	Drivers[1].View_Invalidate						= glide_View_Invalidate;
	Drivers[1].View_Update							= glide_View_Update;
	Drivers[1].Geometry_Initialize					= glide_Geometry_Initialize;
	Drivers[1].Geometry_Deinitialize				= glide_Geometry_Deinitialize;
	Drivers[1].Geometry_Invalidate					= glide_Geometry_Invalidate;
	Drivers[1].Geometry_Update						= glide_Geometry_Update;
	Drivers[1].Geometry_SetVector					= glide_Geometry_SetVector;
	Drivers[1].Camera_Initialize					= glide_Camera_Initialize;
	Drivers[1].Camera_Deinitialize					= glide_Camera_Deinitialize;
	Drivers[1].Camera_Invalidate					= glide_Camera_Invalidate;
	Drivers[1].Camera_Render						= glide_Camera_Render;
	Drivers[1].Camera_SetFieldOfView				= glide_Camera_SetFieldOfView;
	Drivers[1].Console_Initialize					= glide_Console_Initialize;
	Drivers[1].Console_Deinitialize					= glide_Console_Deinitialize;
	Drivers[1].Console_Invalidate					= glide_Console_Invalidate;
	Drivers[1].Console_Render						= glide_Console_Render;
	Drivers[1].LightPoint_Initialize				= glide_LightPoint_Initialize;
	Drivers[1].LightPoint_Deinitialize				= glide_LightPoint_Deinitialize;
	Drivers[1].LightPoint_SetConstantAttenuation	= glide_LightPoint_SetConstantAttenuation;
	Drivers[1].LightPoint_SetLinearAttenuation		= glide_LightPoint_SetLinearAttenuation;
	Drivers[1].LightPoint_SetQuadraticAttenuation	= glide_LightPoint_SetQuadraticAttenuation;
	Drivers[1].LightPoint_SetAmbientIntensity		= glide_LightPoint_SetAmbientIntensity;
	Drivers[1].LightPoint_SetDiffuseIntensity		= glide_LightPoint_SetAmbientIntensity;
	Drivers[1].LightPoint_SetSpecularIntensity		= glide_LightPoint_SetAmbientIntensity;
	Drivers[1].IndexedPolygon_Initialize			= glide_IndexedPolygon_Initialize;
	Drivers[1].IndexedPolygon_Deinitialize			= glide_IndexedPolygon_Deinitialize;
	Drivers[1].IndexedPolygon_CachePolygon			= glide_IndexedPolygon_CachePolygon;
	Drivers[1].IndexedPolygon_SetIteration			= glide_IndexedPolygon_SetIteration;
	Drivers[1].Texture_Initialize					= glide_Texture_Initialize;
	Drivers[1].Texture_Deinitialize					= glide_Texture_Deinitialize;
	Drivers[1].Texture_SetFrame						= glide_Texture_SetFrame;
	Drivers[1].Bitmap_Initialize					= glide_Bitmap_Initialize;
	Drivers[1].Bitmap_Deinitialize					= glide_Bitmap_Deinitialize;
	Drivers[1].Bitmap_Invalidate					= glide_Bitmap_Invalidate;
	Drivers[1].Bitmap_Render						= glide_Bitmap_Render;
	// OpenGL driver
	Drivers[2].Initialize							= gl_Initialize;
	Drivers[2].Deinitialize							= gl_Deinitialize;
	Drivers[2].WindowScreen_Initialize				= gl_WindowScreen_Initialize;
	Drivers[2].WindowScreen_Deinitialize			= gl_WindowScreen_Deinitialize;
	Drivers[2].WindowScreen_Update					= gl_WindowScreen_Update;
	Drivers[2].WindowScreen_GetWidth				= gl_WindowScreen_GetWidth;
	Drivers[2].WindowScreen_GetHeight				= gl_WindowScreen_GetHeight;
	Drivers[2].WindowScreen_CycleMode				= gl_WindowScreen_CycleMode;
	Drivers[2].WindowScreen_Lock					= gl_WindowScreen_Lock;
	Drivers[2].WindowScreen_Unlock					= gl_WindowScreen_Unlock;
	Drivers[2].View_Initialize						= gl_View_Initialize;
	Drivers[2].View_Deinitialize					= gl_View_Deinitialize;
	Drivers[2].View_Invalidate						= gl_View_Invalidate;
	Drivers[2].View_Update							= gl_View_Update;
	Drivers[2].Geometry_Initialize					= gl_Geometry_Initialize;
	Drivers[2].Geometry_Deinitialize				= gl_Geometry_Deinitialize;
	Drivers[2].Geometry_Invalidate					= gl_Geometry_Invalidate;
	Drivers[2].Geometry_Update						= gl_Geometry_Update;
	Drivers[2].Geometry_SetVector					= gl_Geometry_SetVector;
	Drivers[2].Camera_Initialize					= gl_Camera_Initialize;
	Drivers[2].Camera_Deinitialize					= gl_Camera_Deinitialize;
	Drivers[2].Camera_Invalidate					= gl_Camera_Invalidate;
	Drivers[2].Camera_Render						= gl_Camera_Render;
	Drivers[2].Camera_SetFieldOfView				= gl_Camera_SetFieldOfView;
	Drivers[2].Console_Initialize					= gl_Console_Initialize;
	Drivers[2].Console_Deinitialize					= gl_Console_Deinitialize;
	Drivers[2].Console_Invalidate					= gl_Console_Invalidate;
	Drivers[2].Console_Render						= gl_Console_Render;
	Drivers[2].LightPoint_Initialize				= gl_LightPoint_Initialize;
	Drivers[2].LightPoint_Deinitialize				= gl_LightPoint_Deinitialize;
	Drivers[2].LightPoint_SetConstantAttenuation	= gl_LightPoint_SetConstantAttenuation;
	Drivers[2].LightPoint_SetLinearAttenuation		= gl_LightPoint_SetLinearAttenuation;
	Drivers[2].LightPoint_SetQuadraticAttenuation	= gl_LightPoint_SetQuadraticAttenuation;
	Drivers[2].LightPoint_SetAmbientIntensity		= gl_LightPoint_SetAmbientIntensity;
	Drivers[2].LightPoint_SetDiffuseIntensity		= gl_LightPoint_SetAmbientIntensity;
	Drivers[2].LightPoint_SetSpecularIntensity		= gl_LightPoint_SetAmbientIntensity;
	Drivers[2].IndexedPolygon_Initialize			= gl_IndexedPolygon_Initialize;
	Drivers[2].IndexedPolygon_Deinitialize			= gl_IndexedPolygon_Deinitialize;
	Drivers[2].IndexedPolygon_CachePolygon			= gl_IndexedPolygon_CachePolygon;
	Drivers[2].IndexedPolygon_SetIteration			= gl_IndexedPolygon_SetIteration;
	Drivers[2].Texture_Initialize					= gl_Texture_Initialize;
	Drivers[2].Texture_Deinitialize					= gl_Texture_Deinitialize;
	Drivers[2].Texture_SetFrame						= gl_Texture_SetFrame;
	Drivers[2].Bitmap_Initialize					= gl_Bitmap_Initialize;
	Drivers[2].Bitmap_Deinitialize					= gl_Bitmap_Deinitialize;
	Drivers[2].Bitmap_Invalidate					= gl_Bitmap_Invalidate;
	Drivers[2].Bitmap_Render						= gl_Bitmap_Render;

	Cache = Drivers[ActiveDriver].Initialize();

	KeyB = new Keyboard();
	AddDevice((InputDevice*)KeyB);

	// Setup qsound driver
	qmix = new AudioDevice();
	qmix->CreateVirtual("Devices/Audio/QMixer");
	qmix->Initialize	= qmix_SoundPoint_Initialize;
	qmix->Deinitialize	= qmix_SoundPoint_Deinitialize;
	qmix->SetPosition	= qmix_SoundPoint_SetPosition;

	// Setup a defualt sound device
	def = new AudioDevice("Devices/Audio/QMixer");
	def->CreateVirtual("Devices/Audio/Default");

	Application(argc,argv);
}
Native::~Native(){
	__global__Native__=0;
	Drivers[ActiveDriver].Deinitialize(Cache);
	Free(Drivers);
	delete KeyB;
	delete qmix;
	delete def;
	delete ScreenArray;
	delete LightPointArray;
	delete MaterialArray;
	delete InputDevices;
	delete host;
delete server;
}

class Host *Native::GetHost(void){
	return(host);
}

void Native::AddWindowScreen(WindowScreen *scr){
	ScreenArray->Add(scr);
}
void Native::AddLightPoint(class LightPoint *lp){
	LightPointArray->Add(lp);
}
void Native::AddMaterial(class Material *mat){
	MaterialArray->Add(mat);
}

void Native::AddDevice(class InputDevice *dev){
	InputDevices->Add(dev);
}

void Native::SendKeyDown(uint8 key){
//	host->SendMessage(KeyDownHook,(char*)&key);
	Keyboard *kb = (Keyboard*)InputDevices->GetItem(0);
	kb->WindowsKey(true,key);
}
void Native::SendKeyUp(uint8 key){
//	host->SendMessage(KeyUpHook,(char*)&key);
	Keyboard *kb = (Keyboard*)InputDevices->GetItem(0);
	kb->WindowsKey(false,key);
}

class DynamicArray *Native::GetLightPointArray(void){
	return(LightPointArray);
}

double Native::Time(void){		// Return time in seconds.
	return(((double)(clock()-StartTime))*(1.0f/CLOCKS_PER_SEC));
}

// Keyboard input device

Keyboard::Keyboard() : InputDevice(){

	SetName("/Devices/Input/Keyboard");

	char foo[256][16] = {
		 "0x00","0x01","0x02","0x03","0x04","0x05","0x06","0x07",
		 "0x08","0x09","0x0a","0x0b","0x0c","0x0d","0x0e","0x0f",
		"Shift","0x11","0x12","0x13","0x14","0x15","0x16","0x17",
		 "0x18","0x19","0x1a", "Esc","0x1c","0x1d","0x1e","0x1f",
		 "0x20","0x21","0x22","0x23","0x24","0x25","0x26","0x27",
		 "0x28","0x29","0x2a","0x2b","0x2c","0x2d","0x2e","0x2f",
		    "0",   "1",   "2",   "3",   "4",   "5",   "6",   "7",
		    "8",   "9","0x3a","0x3b","0x3c","0x3d","0x3e","0x3f",
		 "0x40","0x41","0x42","0x43","0x44","0x45","0x46","0x47",
		 "0x48","0x49","0x4a","0x4b","0x4c","0x4d","0x4e","0x4f",
		 "0x50","0x51","0x52","0x53","0x54","0x55","0x56","0x57",
		 "0x58","0x59","0x5a","0x5b","0x5c","0x5d","0x5e","0x5f",
		 "0x60",   "a",   "b",   "c",   "d",   "e",   "f",   "g",
		    "h",   "i",   "j",   "k",   "l",   "m",   "n",   "o",
		    "p",   "q",   "r",   "s",   "t",   "u",   "v",   "w",
		    "x",   "y",   "z","0x7b","0x7c","0x7d","0x7e","0x7f",
		// Shift
		 "0x80","0x81","0x82","0x83","0x84","0x85","0x86","0x87",
		 "0x88","0x89","0x8a","0x8b","0x8c","0x8d","0x8e","0x8f",
		 "0x90","0x91","0x92","0x93","0x94","0x95","0x96","0x97",
		 "0x98","0x99","0x9a","0x9b","0x9c","0x9d","0x9e","0x9f",
		 "0xa0","0xa1","0xa2","0xa3","0xa4","0xa5","0xa6","0xa7",
		 "0xa8","0xa9","0xaa","0xab","0xac","0xad","0xae","0xaf",
		    "!",   "@",   "#",   "$",   "%",   "&",   "/",   "(",
		    ")",   "=","0xba","0xbb","0xbc","0xbd","0xbe","0xbf",
		 "0xc0","0xc1","0xc2","0xc3","0xc4","0xc5","0xc6","0xc7",
		 "0xc8","0xc9","0xca","0xcb","0xcc","0xcd","0xce","0xcf",
		 "0xd0","0xd1","0xd2","0xd3","0xd4","0xd5","0xd6","0xd7",
		 "0xd8","0xd9","0xda","0xdb","0xdc","0xdd","0xde","0xdf",
		 "0xe0",   "A",   "B",   "C",   "D",   "E",   "F",   "G",
		    "H",   "I",   "J",   "K",   "L",   "M",   "N",   "O",
		    "P",   "Q",   "R",   "S",   "T",   "U",   "V",   "W",
		    "X",   "Y",   "Z","0xfb","0xfc","0xfd","0xfe","0xff"
		};
	for(int n=0; n<256; n++){
		sprintf(Keys[n],"%s",foo[n]);
	}
}

Keyboard::~Keyboard(){
}

int Keyboard::GetDigitalPortCount(void){
	return 256;
}

//void Keyboard::DigitalTogle(bool state, char *key){
//	printf("%s %s\n",key,state?"down":"up");
//}

void Keyboard::WindowsKey(bool state, uint8 key){
	DigitalTogle(state,Keys[key]);
}

	//AddDevice(Keyb,"Keyboard");
	//Bind("/Devices/Input/Keyboard","/Devices/Input/Default");

//static bool quit;

//#include <CRTDBG.h>
/*
#ifdef _DEBUG


int __cdecl DummyAllocHook(int nAllocType, void *pvData, size_t nSize, int nBlockUse, long lRequest, const char *szFileName, int nLine){
	return true;
}
*/
//char *deb;
/*
#include <string.h>

int firsthook = 0;

class DynamicArray *AllocArray;

struct AllocBlock{
	int		Request;
	char	Filename[1024];
	int		Line;
};

static bool report=true;

int __cdecl MyAllocHook(int nAllocType, void *pvData, size_t nSize, int nBlockUse, long lRequest, const char *szFileName, int nLine){
	char *operation[] = { "", "allocating", "re-allocating", "freeing" };
	char *blockType[] = { "Free", "Normal", "CRT", "Ignore", "Client" };
	if ( nBlockUse == _CRT_BLOCK )   // Ignore internal C runtime library allocations
		return(true);
	_ASSERT( ( nAllocType > 0 ) && ( nAllocType < 4 ) );
	_ASSERT( ( nBlockUse >= 0 ) && ( nBlockUse < 5 ) );
_CrtSetAllocHook(DummyAllocHook);
	if(report){
		report = false;
		if(firsthook==0) firsthook=lRequest;
		//static int n=0;
		//if(n++>50)
		//sprintf(&deb[strlen(deb)], "Memory operation in %s, line %d: %s a %d-byte '%s' block (# %ld)\n\0",szFileName, nLine, operation[nAllocType], nSize, blockType[nBlockUse], lRequest );
		//if ( pvData != NULL ) fprintf( logFile, " at %X", pvData );
		if(nAllocType!=3){
	int cnt=AllocArray->GetCount();
	for(int n=0; n<cnt; n++){
		if(lRequest==((AllocBlock*)AllocArray->GetItem(n))->Request){
			sprintf(&deb[strlen(deb)],"Warning! Found: %s\n",((AllocBlock*)AllocArray->GetItem(n))->Filename);
			return false;
		}
	}
			AllocBlock *ab = new AllocBlock;
			ab->Request=lRequest;
			sprintf(ab->Filename,"%s",szFileName);
			ab->Line=nLine;
			AllocArray->Add(ab);
		}
		report = true;
	}
_CrtSetAllocHook(MyAllocHook);
	return(true);         // Allow the memory operation to proceed
}

*/
#include <string.h>

int __cdecl MyReportHook(int nAllocType, char *msg, int *retval){
//	static bool output=false;
//	char *str=msg;
//	char nrstr[10000];
//	while(*str!='{')
//		if(*str=='<'){
//			sprintf(&deb[strlen(deb)],"%s\0",msg);
/*			return false;
		}else if(*str++==0){
			if(output){
				//sprintf(&deb[strlen(deb)],"{%s}\0",msg);
				output=false;
			}
			return false;
		}
	int var;
	int n=0;
	str++;
	while(*str!='}') nrstr[n++]=*str++;
	nrstr[n]=0;
	sscanf(nrstr,"%d",&var);
//	if(var>=firsthook){
//		output=true;
//		sprintf(&deb[strlen(deb)],"%s\0",msg);
//	}
	// Find in array
	int cnt=AllocArray->GetCount();
	for(n=0; n<cnt; n++){
		if(var==((AllocBlock*)AllocArray->GetItem(n))->Request){
			sprintf(&deb[strlen(deb)],"Found: %s\n",((AllocBlock*)AllocArray->GetItem(n))->Filename);
			return false;
		}
	}
*/	return false;
}

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <float.h>

//#endif

HANDLE Win_Console;
void Win_Printf(char *str){
	unsigned long var;
	WriteConsole(Win_Console,str,strlen(str),&var,0);
}
void Win_Puts(char *str){
	unsigned long var;
	WriteConsole(Win_Console,str,strlen(str),&var,0);
}

void fake_main(int argc, char **argv) {

	_control87( //fp(
		_EM_INVALID|
		_EM_DENORMAL|
		_EM_ZERODIVIDE|
		_EM_OVERFLOW|
		_EM_UNDERFLOW|
		_EM_INEXACT,_MCW_EM);

//	unsigned long var;
//	WriteConsole(Console,"Hi!\n",4,&var,0);

#ifdef _DEBUG
	void Initialize_Debug(void);
	Initialize_Debug();
#endif

#ifdef _DEBUG
//	deb = (char*)malloc(1024*1024);
//	sprintf(deb,"\0");
//	AllocArray = new DynamicArray;
#endif

	ActiveDriver=1;
//	int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
//	_CrtSetDbgFlag(tmpFlag|_CRTDBG_LEAK_CHECK_DF|_CRTDBG_CHECK_CRT_DF);
//	_CrtSetAllocHook(MyAllocHook);
//	_CrtSetReportHook(MyReportHook);

	delete new Native(argc,argv);

//	_CrtSetDbgFlag(tmpFlag);
//	_CrtDumpMemoryLeaks();
//#ifdef _DEBUG
//	MessageBox(0,deb,0,0);
//#endif
#ifdef _DEBUG
	void Deinitialize_Debug(void);
//	MessageBox(0,Deinitialize_Debug(),0,0);
	Deinitialize_Debug();
#endif
//	quit=true;
}

// Windows junk starts here

#ifdef _WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
HWND hWndMain;

#include <sys/timeb.h>

static char    *fakeName = "WinTest";
static char    *argvbuf[32];
static char    cmdLineBuffer[1024];
char **commandLineToArgv(LPSTR lpCmdLine, int *pArgc){
    char    *p, *pEnd;
    int     argc = 0;

    argvbuf[argc++] = fakeName;
    if (lpCmdLine == NULL) {
        *pArgc = argc;
         return argvbuf;
    }
    strcpy(cmdLineBuffer, lpCmdLine);
    p = cmdLineBuffer;
    pEnd = p + strlen(cmdLineBuffer);
    if (pEnd >= &cmdLineBuffer[1022]) pEnd = &cmdLineBuffer[1022];
    while (1) {
        while (*p == ' ') p++;
        if (p >= pEnd) break;
        argvbuf[argc++] = p;
        if (argc >= 32) break;
        while (*p && (*p != ' ')) p++;
        if (*p == ' ') *p++ = 0;
    }
    *pArgc = argc;
    return argvbuf;
}

uint8 keystates[256/8];

long FAR PASCAL MainWndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    PAINTSTRUCT ps;
    HDC         hdc;

    switch(message){
//    case WM_SETCURSOR:
//        if (Res != GR_RESOLUTION_NONE) {
//            SetCursor(NULL);
//            return 0;
//        }
//    break;
    case WM_CREATE:
		memset(keystates,0,256/8);
    break;
    case WM_PAINT:
        hdc = BeginPaint( hWnd, &ps );
        EndPaint( hWnd, &ps );
    return 1;
//    case WM_CLOSE:
//        queue[qhead++] = 'q'; qhead &= 255;
//    break;
    case WM_DESTROY:
		PostQuitMessage(0);
    break;
    case WM_MOVE:
//        if(!grSstControl(GR_CONTROL_MOVE)){
//            PostMessage( hWndMain, WM_CLOSE, 0, 0 );
//            return 0;
//        }
	break;
    case WM_DISPLAYCHANGE:
    case WM_SIZE:
//        {
//            extern void getWindowSize(float *width, float *height);
//            float  width, height;
//
//            getWindowSize(&width, &height);
//            tlSetScreen( width, height);
//        }
//        if (!grSstControl(GR_CONTROL_RESIZE)) {
//            PostMessage( hWndMain, WM_CLOSE, 0, 0 );
//            return 0;
//        }
//struct _timeb tstruct;
    break;
	case WM_KEYDOWN:
		if(!(keystates[wParam/8]&(1<<(wParam&7)))){
			keystates[wParam/8]|=(1<<(wParam&7));
			if(__global__Native__){
				if((wParam>='A')&&(wParam<='Z')) __global__Native__->SendKeyDown(wParam-'A'+'a');
				else __global__Native__->SendKeyDown(wParam);
			}
		}
	break;
	case WM_KEYUP:
		if(keystates[wParam/8]&(1<<(wParam&7))){
			if(__global__Native__){
				if((wParam>='A')&&(wParam<='Z')) __global__Native__->SendKeyUp(wParam-'A'+'a');
				else __global__Native__->SendKeyUp(wParam);
			}
		}
		keystates[wParam/8]&=~(1<<(wParam&7));
	break;
    case WM_CHAR:
		if (!isascii(wParam)) break;
#if 0
        printf("Posting keystroke %.02x\n", wParam);
        fflush(stdout);
#endif
//        queue[qhead++] = tolower(wParam); 
//        qhead &= 255;
    break;
    default:
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);

}

static BOOL initApplication( HANDLE hInstance, int nCmdShow ){
    WNDCLASS    wc;
    BOOL        rc;

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = MainWndproc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION);    /* generic icon */
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( BLACK_BRUSH );
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = "WinClass";
    rc = RegisterClass( &wc );
    if(!rc) return false;

    hWndMain = CreateWindowEx(
#if 0                         
        WS_EX_APPWINDOW  gives you regular borders?
        WS_EX_TOPMOST    Works as advertised.
#endif
        WS_EX_APPWINDOW,
        "WinClass",
        "Copyright (C) 1997 RealityMakers Entertainment AS. All rights reserved.",
        WS_OVERLAPPED |     
            WS_CAPTION  |     
            WS_THICKFRAME | 
            WS_MAXIMIZEBOX | 
            WS_MINIMIZEBOX | 
            WS_VISIBLE |    /* so we don't have to call ShowWindow */
            WS_POPUP |      /* non-app window */
            WS_SYSMENU,     /* so we get an icon in the tray */
        CW_USEDEFAULT, 
        CW_USEDEFAULT,
        640,                /* GetSystemMetrics(SM_CXSCREEN), */
         40,                /* GetSystemMetrics(SM_CYSCREEN), */
        NULL,
        NULL,
        hInstance,
        NULL );

    if(!hWndMain) return false;
    ShowWindow( hWndMain, SW_NORMAL);
    UpdateWindow( hWndMain );
    return true;
}

struct ThreadArgs{
	int		argc;
	char	**argv;
};

void WindowThread(ThreadArgs *a){
	fake_main(a->argc, a->argv);
	SendMessage(hWndMain,WM_DESTROY,0,0);
}

/*
HANDLE APCHandle;

static void APCThread(void){
	SleepEx(INFINITE,true);
}
*/

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){

	AllocConsole();
	Win_Console = CreateConsoleScreenBuffer(GENERIC_WRITE,0,0,CONSOLE_TEXTMODE_BUFFER,0); 
	SetConsoleActiveScreenBuffer(Win_Console);
	COORD Size = {600,1000};
	SetConsoleScreenBufferSize(Win_Console,Size);

	__global__Native__=0;
//	quit=false;
    if(!initApplication(hInstance, nCmdShow)) return FALSE;
    int     argc;
    char    **argv;
    argv = commandLineToArgv(lpCmdLine, &argc);

	ThreadArgs args={argc,argv};
	DWORD dwThreadID = 0;
	HANDLE hThread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)WindowThread,
		&args,
		0,
		&dwThreadID);

//	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);
/*
	APCHandle = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)APCThread,
		0,
		0,
		&dwThreadID);
*/
	MSG msg;
	while(GetMessage(&msg,NULL,0,0)){ 
        TranslateMessage(&msg); 
        DispatchMessage(&msg);
	}

	CloseHandle(Win_Console);
	FreeConsole();

    DestroyWindow(hWndMain);
    return 0;
}

//int errno = 0;

#endif

#endif
