
CC	= gcc
DEFS	= -D_DEBUG
ALL	= libinterface.a
AR	= ar

interface_OBJECTS = \
	ARGB.o Bitmap.o BitmapLoader.o
# ConsoleStream.o Debug.o DynamicArray.o \
#	DynamicBuffer.o Font.o Garbage.o Kernel.o KernelClient.o Native.o Rect.o \
#	Semaphore.o String.o View.o WindowScreen.o rme_Memory.o time.o win32_Specific.o \
#	win32_Thread.o
interface_SOURCES = \
	../ARGB.cpp ../Bitmap.cpp ../BitmapLoader.cpp
# ../ConsoleStream.cpp ../Debug.cpp ../DynamicArray.cpp \
#	../DynamicBuffer.cpp ../Font.cpp ../Garbage.cpp ../Kernel.cpp ../KernelClient.cpp ../Native.cpp ../Rect.cpp \
#	../Semaphore.cpp ../String.cpp ../View.cpp ../WindowScreen.cpp ../rme_Memory.cpp ../time.cpp ../win32_Specific.cpp \
#	../win32_Thread.cpp

INCLUDES = -I../../ -I../Loaders/

all:	$(ALL)

#.cpp.o:
#	$(CC) -c $(DEFS) $(INCLUDES) $@

libinterface.a: $(interface_OBJECTS)
	rm -f libinterface.a
	$(AR) cru libinterface.a $(interface_OBJECTS)

$(interface_OBJECTS): $(interface_SOURCES)
	@for file in $(interface_SOURCES); do \
		echo $(CC) -c $(DEFS) $(INCLUDES) $$file;\
		$(CC) -w -c $(DEFS) $(INCLUDES) $$file;\
	done
