/**************************************************************\
|      Copyright (C) 1997 RealityMakers Entertainment AS       |
|                     All rights reserved.                     |
\**************************************************************/

Obsolete file.

#ifndef NATIVE_H
#define NATIVE_H

typedef signed		char	int8;
typedef signed		short	int16;
//typedef signed		int		int32;
typedef long int32;
typedef unsigned	char	uint8;
typedef unsigned	short	uint16;
typedef unsigned	int		uint32;

#include "Message.h"

class Native : public Message{
public:			Native(int argc, char **argv);
				~Native();
class	Host	*GetHost(void);
		void	*CreateHook(char *);
		void	*GetHook(char *);
		void	SendKeyDown(uint8 Key);
		void	SendKeyUp(uint8 Key);
class DynamicArray *GetLightPointArray(void);	// fixme (remove or private)
		double	Time(void);
		void	AddDevice(class InputDevice *);
private:
friend class WindowScreen;
friend class Node;
friend class View;
class	Server	*server;
		int		KeyDownHook;
		int		KeyUpHook;
class	DynamicArray	*ScreenArray;
class	DynamicArray	*LightPointArray;
class	DynamicArray	*MaterialArray;
class	DynamicArray	*InputDevices;
		void	AddWindowScreen(class WindowScreen *);
		void	AddLightPoint(class LightPoint *);
		void	AddMaterial(class Material *);
		void	*Cache;
class Keyboard	*KeyB;
class AudioDevice *qmix;
class AudioDevice *def;
};
#endif
