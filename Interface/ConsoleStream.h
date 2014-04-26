
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

//#include <Drivers/ViewDriver.h>

class ConsoleStream{ // : public ViewDriver{
public:			ConsoleStream(class Rect &);
				~ConsoleStream();
		void	Clear(void);
		void	_Apply(class Font *);
class	Font	*GetFont(void);
		void	PutChar(char);
		int		Print(const char *,...);
		char	*GetString(void);
//		int		vPrint(char *,va_list);
		// fixme (We only have a pointer to the stream!)
		void	operator<<(char *);
		void	operator<<(class Vector &);
		void	operator<<(class Quaternion &);
		void	operator<<(class Matrix3x3 &);
		void	Dump(void);	// Dumps the textbuffer to stdout
protected:
class	Semaphore		*sem;
class	DynamicBuffer	*Buffer;
		void	_ConsoleStream_SetView(class View *view);
private:
		void	_ConsoleStream_Invalidate(void);
		int		LineCount;
class	Font	*font;
};
