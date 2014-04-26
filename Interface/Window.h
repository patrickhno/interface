
#include <Interface/View.h>

class Workspace;

class protocol_Window{
protected:
virtual				Window(class Rect &rect) = 0;
virtual	Workspace	*GetWorkspace() = 0;
};

class Window : View, protocol_Window{
public:				Window(Rect &rect);
					~Window();
class	Workspace	*GetWorkspace();
private:
		Workspace	*workspace;
};

