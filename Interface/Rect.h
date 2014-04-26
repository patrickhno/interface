
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#ifndef RECT_H
#define RECT_H

class Rect{
public:			Rect();
				Rect(float x, float y, float rx, float ly);		// Left x, Top y, Right x, Bottom y
				Rect(float rx, float ry);
		Rect	&operator=(Rect &);
		float	x,y,rx,ly,Width,Height;
};

#endif
