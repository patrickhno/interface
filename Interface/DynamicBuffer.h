
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#ifndef DYNAMICBUFFER_H
#define DYNAMICBUFFER_H

#define DBUF_SHARED 1

class DynamicBuffer{
public:			DynamicBuffer();
				DynamicBuffer(int BlockSize);
				DynamicBuffer(int BlockSize, int flags);
				DynamicBuffer(DynamicBuffer *);
				~DynamicBuffer();
		#ifdef _DEBUG
		void	Add(void *Buffer, int Size, char *,int);
		void	Add(void *Buffer, int Size){ Add(Buffer,Size,__FILE__,__LINE__); }
		void	*GetBuffer(char *, int);
		void	*GetBuffer(void){ return GetBuffer(__FILE__,__LINE__); };
		#else
		void	Add(void *Buffer, int Size);	// Copy Size bytes from Buffer to the end of our buffer
		void	*GetBuffer(void);
		#endif
//		void	SetBuffer(void *, int);
		int		GetCount(void) const;			// Returns number of bytes in buffer
		void	Cut(int offset, int bytes);		// Cuts out from the buffer
		void	New(void);						// Free's the buffer
private:
		int		Count;
		int		BytesLeft,BufferSize,BlockSize;
		void	*Buffer;
//class	DynamicArray	*Array;
//		int		BytesInArray;
		void	*WorkBuffer;
		int		BytesInWorkBuffer;
		int		flags;
class Semaphore	*sem;
};

#endif
