
#ifdef OPENGL

#include <Interface/Window.h>
#include <Interface/Workspace.h>

Window::Window(Rect &rect) : View(rect){
	Workspace *w = 0;
	workspace = w->GetActiveWorkspace();
	workspace->Apply(this);
}

Window::~Window(){
	workspace->Detatch(this);
}

Workspace *Window::GetWorkspace(){
	return workspace;
}

#endif
