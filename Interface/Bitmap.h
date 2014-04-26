
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#ifndef BITMAP_H
#define BITMAP_H

#include "BitmapLoader.h"
//#include <Drivers/BitmapDriver.h>

//#define BITMAP_RGB	1
//#define BITMAP_ARGB 2

class Bitmap : BitmapLoader{ //, BitmapDriver{
public:			Bitmap(const class String &, const int format);
				Bitmap(class Rect &, int format);
				Bitmap(Bitmap *from, class Rect &);
				Bitmap(Bitmap *);
//				Bitmap(Bitmap *,bool,bool);
//				Bitmap(Bitmap &);
				~Bitmap();
		void	Clear(void);
//		bool	OpenBitmap(char *name);
		int		GetWidth(void);
		int		GetHeight(void);
		int		GetFrameCount(void);
class	ARGB	*LoadBuffer(int frame);
		void	UnloadBuffer(void);
		void	CopyRect(Bitmap *frombm, const class Rect &from, const class Rect &to);
		void	Add(Bitmap *from);
		void	Render(class View *);
		Bitmap	&operator*=(ARGB &);
		Bitmap	&operator|=(ARGB &);
		void	SetAlpha(float);
		void	CalculateAlpha(void);
		int		GetFormat(void);
		void	SetTranslusency(float);
		void	Flush(void);
		void	SetFrame(int);
		int		GetFrame(void) const;
//		void	Print(const class Font *font,const char *fmt,...);
		void	Scale(const Rect &);
		enum{
			cRGB,
			cARGB
		};
private:
		void	AddCache(void){ CacheCount++; }
		void	SubCache(void){ CacheCount--; }
		Bitmap	*Cache;
		int		CacheCount;

		float	translusency;
		int		format;
		ARGB	*_Bitmap;
		int		Width;
		int		Height;
		bool	isfile;
		int		framebuf,count;
		int		frame;
};

#endif

