
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#ifndef BITMAP32_H
#define BITMAP32_H

#include <posix/sys/types.h>
//#include <Drivers/Bitmap32Driver.h>
#include <Interface/String.h>
#include <assert.h>

class Bitmap32{ // : public Bitmap32Driver{
public:				Bitmap32(const class String &, const int);
					Bitmap32(const class Rect &, int);
					Bitmap32(class Bitmap32 *);
					Bitmap32(class Bitmap *);
					~Bitmap32();
		void		Flush(void); //{ Invalidate(); }
		int			GetWidth(void) const{  return Width;  }
		int			GetHeight(void) const{ return Height; }
		int			GetFrame(void) const{  return frame;  }
		int			GetFormat(void) const{ return format; }
		int			GetFrameCount(void) const;
		void		SetFrame(const int f){ assert((f<GetFrameCount())&&(f>=0)); frame = f; }
		void		Clear(void);
class	ARGB32		*LoadBuffer(const int f) const;
		void		UnloadBuffer(void) const{}
		Bitmap32	&operator*=(const class ARGB &col);
		Bitmap32	&operator*=(const class Bitmap32 &bm);
		Bitmap32	&operator*=(class Bitmap &bm);
		void		Multiply(const class ARGB &, const class Rect &);
		void		Add(Bitmap32 *);
		void		Scale(const class Rect &rect);
		void		SetSize(const class Rect &rect);
		void		CopyRect(Bitmap32 *frombm, const class Rect &from, const class Rect &to);
const	String		&GetName(void) const{ return Name; };
		enum{
			cRGB,
			cARGB
		};
private:
		String		Name;
friend	class		View;
		void		Prepare(class View *view){
			//if(Cache){
			//	int tmp = Cache->frame;
			//	Cache->frame = frame;
			//	Cache->Bitmap32Driver::Prepare(view);
			//	Cache->frame = tmp;
			//}else Bitmap32Driver::Prepare(view);
		}
		void		Render(class View *view, const ARGB &translusency){
			//if(Cache){
			//	int tmp = Cache->frame;
			//	Cache->frame = frame;
			//	Cache->Bitmap32Driver::Render(view,translusency);
			//	Cache->frame = tmp;
			//}else Bitmap32Driver::Render(view,translusency);
		};

		void			AddCache(void){ CacheCount++; }
		void			SubCache(void){
							CacheCount--;
							if(!CacheCount) delete this;
						}
		int				CacheCount;

		//Bitmap32		*Cache;
		int				Width,Height,format,frame;
class	DynamicArray	*Frames;
};

#endif

