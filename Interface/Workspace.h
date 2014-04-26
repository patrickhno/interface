
class protocol_Workspace{
protected:
//virtual						Workspace() = 0;
virtual	class	Workspace	*GetActiveWorkspace() = 0;
virtual						Apply(class Window *w) = 0;
virtual						Detatch(class Window *w) = 0;
};

class Workspace : protocol_Workspace{
public:				Workspace();
					~Workspace();
		Workspace	*GetActiveWorkspace();
					Apply(class Window *w);
					Detatch(class Window *w);
};

