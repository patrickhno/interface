
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <windows.h>
#include <posix/unistd.h>
#include <posix/time.h>
#include <posix/stdlib.h>
#include <posix/stdio.h>
#include <posix/string.h>

//#include <Drivers/Drivers.h>
//#include <Drivers/null/null_Drivers.h>
//#include <Drivers/glide/glide_Drivers.h>
//#include <Drivers/opengl/gl_Drivers.h>
#include "Kernel.h"
#include "DynamicArray.h"
#include "WindowScreen.h"
#include <3dapi/network/Host.h>
#include <Devices/AudioDevice.h>
//#include "qmix_QMixer.h"
#include "rme_Memory.h"
#include "./Process.h"
#include <3DAPI/Network/Server.h>
#include <Devices/Input/InputDevice.h>

#include "Debug.h"
#include "globals.h"

void main(int argc, char **argv);

// Global stuff (for now)
Driver *Drivers;
int ActiveDriver;
Kernel *__global__Kernel__ = 0;
Process *ThisProcess;
//__int64 StartTime;
//double Frequency;
//-------------

class Keyboard : InputDevice{
public:			Keyboard();
				~Keyboard();
		int		GetDigitalPortCount(void);
//		int		GetDigitalPortType(int digitalport);
//virtual	void	DigitalTogle(bool,char *);
private:
friend class Kernel;
		void	WindowsKey(bool,uint8);
		char	Keys[256][16];
};

class	Host	*host;

//#include "rme_Memory.h"

Kernel::Kernel(int argc, char **argv) : Message(){

	//WORD	wVersionRequested;
	//wVersionRequested = MAKEWORD(1, 1);
	WSADATA		winsockdata;
	if(WSAStartup(MAKEWORD(2,0),&winsockdata)){
		printf("WSAStartup failed!\n");
		while(true) sleep(10000);
	}

//	extern char root_path[1024];
//	sprintf(root_path,"");

	SetName("Kernel");
	ThisProcess = 0;
//	StartTime=clock();
//	StartTime = -1;
//	Frequency = -1;
	Time();
server = new Server();	// Local server
	host=new Host();
//	KeyDownHook=host->GetHook("+key");
//	KeyUpHook=host->GetHook("-key");
//	ScreenArray = new DynamicArray(DARRAY_GENID);		// fixme: GENID?
//	LightPointArray = new DynamicArray(DARRAY_GENID);	// fixme: GENID?
	MaterialArray = new DynamicArray(DARRAY_GENID);		// fixme: GENID?
	InputDevices = new DynamicArray(DARRAY_GENID);		// fixme: GENID?
	__global__Kernel__=this;
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
	Drivers[0].WindowScreen_SetScale				= null_WindowScreen_SetScale;
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
	Drivers[0].Texture_Invalidate					= null_Texture_Invalidate;
	Drivers[0].Texture_SetFrame						= null_Texture_SetFrame;
	Drivers[0].Texture_CacheAll						= null_Texture_CacheAll;
	Drivers[0].Bitmap_Initialize					= null_Bitmap_Initialize;
	Drivers[0].Bitmap_Deinitialize					= null_Bitmap_Deinitialize;
	Drivers[0].Bitmap_Invalidate					= null_Bitmap_Invalidate;
	Drivers[0].Bitmap_Render						= null_Bitmap_Render;
	Drivers[0].Bitmap32_Initialize					= null_Bitmap32_Initialize;
	Drivers[0].Bitmap32_Deinitialize				= null_Bitmap32_Deinitialize;
	Drivers[0].Bitmap32_Invalidate					= null_Bitmap32_Invalidate;
	Drivers[0].Bitmap32_Prepare						= null_Bitmap32_Prepare;
	Drivers[0].Bitmap32_Render						= null_Bitmap32_Render;
	Drivers[0].Bitmap32_DisableCache				= null_Bitmap32_DisableCache;
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
	Drivers[1].WindowScreen_SetScale				= glide_WindowScreen_SetScale;
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
	Drivers[1].Texture_Invalidate					= glide_Texture_Invalidate;
	Drivers[1].Texture_SetFrame						= glide_Texture_SetFrame;
	Drivers[1].Texture_CacheAll						= glide_Texture_CacheAll;
	Drivers[1].Bitmap_Initialize					= glide_Bitmap_Initialize;
	Drivers[1].Bitmap_Deinitialize					= glide_Bitmap_Deinitialize;
	Drivers[1].Bitmap_Invalidate					= glide_Bitmap_Invalidate;
	Drivers[1].Bitmap_Render						= glide_Bitmap_Render;
	Drivers[1].Bitmap32_Initialize					= glide_Bitmap32_Initialize;
	Drivers[1].Bitmap32_Deinitialize				= glide_Bitmap32_Deinitialize;
	Drivers[1].Bitmap32_Invalidate					= glide_Bitmap32_Invalidate;
	Drivers[1].Bitmap32_Prepare						= glide_Bitmap32_Prepare;
	Drivers[1].Bitmap32_Render						= glide_Bitmap32_Render;
	Drivers[1].Bitmap32_DisableCache				= glide_Bitmap32_DisableCache;
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
	Drivers[2].WindowScreen_SetScale				= gl_WindowScreen_SetScale;
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
	Drivers[2].Texture_Invalidate					= gl_Texture_Invalidate;
	Drivers[2].Texture_SetFrame						= gl_Texture_SetFrame;
	Drivers[2].Texture_CacheAll						= gl_Texture_CacheAll;
	Drivers[2].Bitmap_Initialize					= gl_Bitmap_Initialize;
	Drivers[2].Bitmap_Deinitialize					= gl_Bitmap_Deinitialize;
	Drivers[2].Bitmap_Invalidate					= gl_Bitmap_Invalidate;
	Drivers[2].Bitmap_Render						= gl_Bitmap_Render;
	Drivers[2].Bitmap32_Initialize					= gl_Bitmap32_Initialize;
	Drivers[2].Bitmap32_Deinitialize				= gl_Bitmap32_Deinitialize;
	Drivers[2].Bitmap32_Invalidate					= gl_Bitmap32_Invalidate;
	Drivers[2].Bitmap32_Prepare						= gl_Bitmap32_Prepare;
	Drivers[2].Bitmap32_Render						= gl_Bitmap32_Render;
	Drivers[2].Bitmap32_DisableCache				= gl_Bitmap32_DisableCache;

	Cache = Drivers[ActiveDriver].Initialize();

	KeyB = new Keyboard();
	AddDevice((InputDevice*)KeyB);

	// Setup qsound driver
//	qmix = new AudioDevice();
//	qmix->CreateVirtual("Devices/Audio/QMixer");
//	qmix->_Initialize		= qmix_Initialize;
//	qmix->_Deinitialize		= qmix_Deinitialize;
//	qmix->sp_Initialize		= qmix_SoundPoint_Initialize;
//	qmix->sp_Deinitialize	= qmix_SoundPoint_Deinitialize;
//	qmix->sp_SetPosition	= qmix_SoundPoint_SetPosition;

	// Setup a defualt sound device
//	def = new AudioDevice("Devices/Audio/QMixer");
//	def->CreateVirtual("Devices/Audio/Default");

	extern void main(int,char**);
	main(argc,argv);
}
Kernel::~Kernel(){
	__global__Kernel__=0;
	Drivers[ActiveDriver].Deinitialize(Cache);
	Free(Drivers);
	delete KeyB;
//	delete qmix;
//	delete def;
//	delete ScreenArray;
//	delete LightPointArray;
	delete MaterialArray;
	delete InputDevices;
	delete host;
delete server;
}

class Host *Kernel::GetHost(void){
	return(host);
}

//void Kernel::AddWindowScreen(WindowScreen *scr){
//	#ifdef _DEBUG
//	ScreenArray->Add(scr,__FILE__,__LINE__);
//	#else
//	ScreenArray->Add(scr);
//	#endif
//}
//void Kernel::AddLightPoint(class LightPoint *lp){
//	#ifdef _DEBUG
//	LightPointArray->Add(lp,__FILE__,__LINE__);
//	#else
//	LightPointArray->Add(lp);
//	#endif
//}
void Kernel::AddMaterial(class Material *mat){
	#ifdef _DEBUG
	MaterialArray->Add(mat,__FILE__,__LINE__);
	#else
	MaterialArray->Add(mat);
	#endif
}

void Kernel::AddDevice(class InputDevice *dev){
	#ifdef _DEBUG
	InputDevices->Add(dev,__FILE__,__LINE__);
	#else
	InputDevices->Add(dev);
	#endif
}

void Kernel::SendKeyDown(uint8 key){
//	host->SendMessage(KeyDownHook,(char*)&key);
	Keyboard *kb = (Keyboard*)InputDevices->GetItem(0);
	kb->WindowsKey(true,key);
}
void Kernel::SendKeyUp(uint8 key){
//	host->SendMessage(KeyUpHook,(char*)&key);
	Keyboard *kb = (Keyboard*)InputDevices->GetItem(0);
	kb->WindowsKey(false,key);
}

//class DynamicArray *Kernel::GetLightPointArray(void){
//	return(LightPointArray);
//}

extern bool win32_QueryPerformanceFrequency(__int64 *);
extern bool win32_QueryPerformanceCounter(__int64 *);

double Kernel::Time(void){		// Return time in seconds.
	static __int64 StartTime = -1;
	static float Frequency;
	if((StartTime<0)&&(Frequency<0)){
		__int64 frq;
		if(win32_QueryPerformanceFrequency(&frq)){
			Frequency = float(1.0/double(frq));
			__int64 time;
			win32_QueryPerformanceCounter(&time);
			StartTime = time;
		}else{
			Frequency = -1.0f;
			StartTime = clock();
		}
		return 0;
	}
	if(Frequency>0){
		__int64 time;
		win32_QueryPerformanceCounter(&time);
		return double(time-StartTime)*Frequency*__globals::GetSpeed();
	}else{
		return(double(clock()-StartTime)*(1.0f/CLOCKS_PER_SEC)*__globals::GetSpeed());
	}
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

void Keyboard::WindowsKey(bool state, uint8 key){
	DigitalTogle(state,Keys[key]);
}

#include <exception>

Kernel *Kernel_Initialize(int argc, char **argv) {
	init_Memory();
	#ifdef _DEBUG
	void Initialize_Debug(void);
	Initialize_Debug();
	#endif
	ActiveDriver=1;
	for(int n=0; n<argc; n++){
		//printf("%d: %s\n",n,argv[n]);
		if(!strcmp(argv[n],"-nogfx")) ActiveDriver = 0;
		if(!strcmp(argv[n],"-slow"))  __globals::SetSpeed(.01f);
		if(!strcmp(argv[n],"-fast"))  __globals::SetSpeed(10);
	}
//	try{
		return new Kernel(argc,argv);
/*	}
	catch(Message*){
		printf("Warning: Unhandled message exception in kernel initialize!\n");
	}
	catch(Socket*){
		printf("Warning: Unhandled message exception in kernel initialize!\n");
	}
	catch(class CMemoryException* e ){
		// Handle the out-of-memory exception here.
		printf("Warning: Unhandled out of memory exception!\n");
	}
	catch(class CFileException* e ){
		// Handle the file exceptions here.
		printf("Warning: Unhandled file exception!\n");
	}
	catch(class CException* e ){
		// Handle all other types of exceptions here.
		printf("Warning: Unhandled CException!\n");
	}
	catch(exception *e){
		printf("exception: %s\n",e->what());
	}
	catch(char *str){
		printf("exception: %s\n",str);
	}
	catch(...){
		printf("Warning: Unhandled exception in kernel initialize!\n");
		while(true);
		throw;
	}
*/
}

extern "C" int posix_printf(const char *,...);

void Kernel_Shutdown(Kernel *kernel){
	delete kernel;
	#ifdef _DEBUG
	void Deinitialize_Debug(void);
	Deinitialize_Debug();
	#endif
	deinit_Memory();
	extern void memory_shutdown();
	memory_shutdown();
	#ifdef _DEBUG
	posix_printf("End of kernel reached, waiting forever..\n");
	while(true) sleep(1);
	#endif
}

