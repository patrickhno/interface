
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#ifndef BITMAPLOADER_H
#define BITMAPLOADER_H

class BitmapLoader{
protected:		BitmapLoader();
				~BitmapLoader();
		bool	OpenBitmapFile(char *name);
//		bool	OpenBitmapFile(Bitmap &bm);
		int		GetBitmapWidth(void);
		int		GetBitmapHeight(void);
		int		GetFrameCount(void);
class	ARGB	*LoadFrame(int n);
		void	UnloadFrame(class ARGB *buf);
private:
		void	*Cache;
		void	*(*_Open)(char *name);
		void	 (*__Deinitialize)(void *);
		int		 (*_GetWidth)(void *);
		int		 (*_GetHeight)(void *);
		int		 (*_GetFrameCount)(void *);
class	ARGB	*(*_LoadFrame)(void *, int n);
		void	 (*_UnloadFrame)(void *, class ARGB *buf);
};

#endif

