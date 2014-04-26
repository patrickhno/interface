
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

//#include <Drivers/WindowScreenDriver.h>
#include <Interface/View.h>
#include <Interface/Kernel.h>
#include <3dapi/Network/Message.h>

class WindowScreen : public View{ //, public Client{ //, public Message{
public:			WindowScreen();
				~WindowScreen();
		void	Refresh(void);
		int		GetWidth(void);		// Get width  (int pixels)
		int		GetHeight(void);	// Get height (int pixels)
		void	CycleMode(void);	// Next screen resolution
		uint32	*Lock(void);
		void	Unlock(void);
		void	SetScale(float);
		void	MessageReceived(void);
private:
		//void	__MessageReceived(double time, int Cmd, void *);
		double	otime,timeofs;
};

