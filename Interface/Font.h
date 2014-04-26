
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <posix/sys/types.h>

struct FInfo{
	int32	x;
	int32	y;
	int32	Width;
	int32	Height;
	int32	Baseline;
};

class Font{
public:					Font(const char *name);
						~Font();
		int				GetHeight(void);
class	Bitmap			*GetString(const char *);
class	Bitmap32		*GetString32(const char *);
		Bitmap			*GetBitmap(void);
		FInfo			*GetFontInfo(void);
		float			GetSize(void);
		void			SetSize(float);
		float			GetWidth(const char *str);

class	DynamicArray	*GetCharData(void);

private:
		Font			*Cache;
		int				CacheCount;
		void			AddCache(void){ CacheCount++; };
		void			SubCache(void){ CacheCount--; };
		FInfo			*FontInfo;
		Bitmap			*orgbm;
		Bitmap32		*orgbm32;
		float			Size;

		DynamicArray	*CharData;
};

