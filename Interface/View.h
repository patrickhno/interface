
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#ifndef VIEW_H
#define VIEW_H

#ifdef OPENGL

class Font;

class protocol_View{
protected:
//virtual			View(const class Rect &rect) = 0;
virtual	void	Set(const class Rect &rect) = 0;
virtual	void	SetClearState(const bool state) = 0;
virtual	void	SetClip(const bool state) = 0;
virtual	void	SetColor(const class ARGB &col) = 0;
virtual	Font	*GetFont() = 0;
virtual	void	Apply(const class Font *f) = 0;
virtual	void	Apply(const class Bitmap32 *bm) = 0;
virtual	void	Apply(const class View *v) = 0;
virtual	void	Detatch(const class Bitmap32 *bm) = 0;
virtual	void	Detatch(const class View *v) = 0;
virtual	Rect	*GetAbsolute() = 0;
};

class View : protocol_View{
public:			View(const class Rect &rect);
				~View();
		void	Set(const class Rect &rect);
		void	SetClearState(const bool state);
		void	SetClip(const bool state);
		void	SetColor(const class ARGB &col);
		Font	*GetFont();
		void	Apply(const class Font *f);
		void	Apply(const class Bitmap32 *bm);
		void	Apply(const class View *v);
		void	Detatch(const class Bitmap32 *bm);
		void	Detatch(const class View *v);
		Rect	*GetAbsolute();
};

#else

#include "ConsoleStream.h"
#include "Rect.h"
#include "ARGB.h"
#include "Kernel.h"
#include <3dapi/Network/Client.h>
#include "KernelClient.h"

class View : public ConsoleStream, public KernelClient{
public:			View(class Rect &);
				~View();

		void	Set(class Rect &);
		void	SetClip(const bool state){ clip = state; }

		void	Clear(class WindowScreen *);

		void	Detatch(View *view);

		int		Apply(class View *);		// Apply a child view, and return the ID
		int		Apply(class Bitmap *);
		int		Apply(class Bitmap32 *);
		void	Detatch(class Bitmap32 *);
class WindowScreen *GetScreen(void);
		// Hierarchy (These are just passed over to ViewCamera)
		void	Apply(class Geometry *);
		void	Apply(class Camera *);
		void	Apply(class LightPoint *);
		void	Apply(class SpotLight *);
		void	Apply(class DirectionalLight *);
		void	Apply(class Font *font){ ConsoleStream::_Apply(font);}

		void	Attatch(class Camera *);	// Attatch a camera to this view. (Camera render its display in this view from now on)
		void	SetColor(class ARGB &);
		Rect	*GetRect(void);				// Returns this views rect in parents coordinate system
		Rect	*GetAbsolute(void);			// Returns this views rect in screen coordinates (0.0-1.0)
		Rect	*GetAbsoluteBorder(void);	// Returns this views visible rect in screen coordinates (0.0-1.0)
		ARGB	*GetColor(void);

virtual	void	MessageReceived(double time, int Cmd, void *);
virtual	void	KeyDown(double time, uint8 Key);
virtual	void	KeyUp(double time, uint8 Key);

		void	DisableCollisions(void);	// fixme: This shouldnt be here!!
		void	SetTransparency(float v){ SetTransparency(ARGB(v,v,v,v)); }
		void	SetTransparency(class ARGB &);
class	Camera	*GetViewCamera(void) const{ return ViewCamera; }
private:
		void	SetParent(View *);
class Process *CreatorProcess;
		bool	done;
friend class WindowScreen;
class	WindowScreen	*Screen;
		void	_view_Invalidate(void);
		void	_view_MessageReceived(double time, int Cmd, void *);
		void	SetScreen(class WindowScreen *);
		bool	RefreshView(int pass, double time, class WindowScreen *);
		void	SetAbsolute(class Rect &);
//		void	ReceiveMessage(float time, void *Cmd, void *);
		Rect	Border,Absolute;
		ARGB	Color;
		Rect	ViewRect;
class	Camera	*DummyCamera;
class	Camera	*ViewCamera;
class	DynamicArray	*ChildViews,*DetatchViews;
		int		KeyDownHook;
		int		KeyUpHook;
class	DynamicArray	*Bitmaps,*Bitmaps32;
		View	*Parent;
		bool	collisions;
		ARGB	transparency;
		bool	clip;
};
#endif

#endif
