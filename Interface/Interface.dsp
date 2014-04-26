# Microsoft Developer Studio Project File - Name="Interface" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Interface - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Interface.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Interface.mak" CFG="Interface - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Interface - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Interface - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "Interface - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W2 /GX /Z7 /O2 /I "d:\develop\Metropol\projects\reality\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"c:\develop\lib\Interface.lib"

!ELSEIF  "$(CFG)" == "Interface - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /Z7 /Od /I "d:\develop\Metropol\projects\reality\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_NTSDK" /D "_MT" /FR /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Interface - Win32 Release"
# Name "Interface - Win32 Debug"
# Begin Group "Loaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Loaders\PngLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\Loaders\PngLoader.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ARGB.cpp
# End Source File
# Begin Source File

SOURCE=.\ARGB.h
# End Source File
# Begin Source File

SOURCE=.\ARGB32.h
# End Source File
# Begin Source File

SOURCE=.\Bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Bitmap.h
# End Source File
# Begin Source File

SOURCE=.\Bitmap32.cpp
# End Source File
# Begin Source File

SOURCE=.\Bitmap32.h
# End Source File
# Begin Source File

SOURCE=.\BitmapLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapLoader.h
# End Source File
# Begin Source File

SOURCE=.\ConsoleStream.cpp
# End Source File
# Begin Source File

SOURCE=.\ConsoleStream.h
# End Source File
# Begin Source File

SOURCE=.\Debug.cpp

!IF  "$(CFG)" == "Interface - Win32 Release"

!ELSEIF  "$(CFG)" == "Interface - Win32 Debug"

# ADD CPP /G6 /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Debug.h
# End Source File
# Begin Source File

SOURCE=.\DynamicArray.cpp
# End Source File
# Begin Source File

SOURCE=.\DynamicArray.h
# End Source File
# Begin Source File

SOURCE=.\DynamicBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\DynamicBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Font.cpp
# End Source File
# Begin Source File

SOURCE=.\Font.h
# End Source File
# Begin Source File

SOURCE=.\Garbage.cpp
# End Source File
# Begin Source File

SOURCE=.\Garbage.h
# End Source File
# Begin Source File

SOURCE=..\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\Kernel.cpp
# End Source File
# Begin Source File

SOURCE=.\Kernel.h
# End Source File
# Begin Source File

SOURCE=.\KernelClient.cpp
# End Source File
# Begin Source File

SOURCE=.\KernelClient.h
# End Source File
# Begin Source File

SOURCE=.\Process.cpp
# End Source File
# Begin Source File

SOURCE=.\Process.h
# End Source File
# Begin Source File

SOURCE=.\Rect.cpp
# End Source File
# Begin Source File

SOURCE=.\Rect.h
# End Source File
# Begin Source File

SOURCE=.\rme_Memory.cpp
# End Source File
# Begin Source File

SOURCE=.\rme_Memory.h
# End Source File
# Begin Source File

SOURCE=.\Semaphore.cpp
# End Source File
# Begin Source File

SOURCE=.\Semaphore.h
# End Source File
# Begin Source File

SOURCE=.\String.cpp
# End Source File
# Begin Source File

SOURCE=.\String.h
# End Source File
# Begin Source File

SOURCE=.\Thread.h
# End Source File
# Begin Source File

SOURCE=.\View.cpp
# End Source File
# Begin Source File

SOURCE=.\View.h
# End Source File
# Begin Source File

SOURCE=.\win32_Specific.cpp
# End Source File
# Begin Source File

SOURCE=.\win32_Thread.cpp
# End Source File
# Begin Source File

SOURCE=.\Window.cpp
# End Source File
# Begin Source File

SOURCE=.\Window.h
# End Source File
# Begin Source File

SOURCE=.\WindowScreen.cpp
# End Source File
# Begin Source File

SOURCE=.\WindowScreen.h
# End Source File
# Begin Source File

SOURCE=.\Workspace.cpp
# End Source File
# Begin Source File

SOURCE=.\Workspace.h
# End Source File
# End Target
# End Project
