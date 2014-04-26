
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include "Debug.h"
#include "Rect.h"

Rect::Rect(){
	x=y=rx=ly=Width=Height=0;
}

Rect::Rect(float _x, float _y, float _rx, float _ly){
	x=_x; y=_y;
	rx=_rx; ly=_ly;
	Width=rx-x; Height=ly-y;
}

Rect::Rect(float _rx, float _ly){
	x=0; y=0;
	rx=_rx; ly=_ly;
	Width=rx-x; Height=ly-y;
}

Rect &Rect::operator=(Rect &src){
	x=src.x;
	y=src.y;
	rx=src.rx;
	ly=src.ly;
	Width=src.Width;
	Height=src.Height;
	return *this;
}

