
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

		void*	__cdecl	png_Open(char*);
		void	__cdecl	png_Deinitialize(void*);
		int		__cdecl	png_GetWidth(void*);
		int		__cdecl	png_GetHeight(void*);
		int		__cdecl	png_GetFrameCount(void*);
class	ARGB*	__cdecl	png_LoadFrame(void*, int index);
		void	__cdecl	png_UnloadFrame(void *, ARGB*);
