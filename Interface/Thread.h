
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#ifndef THREAD_H
#define THREAD_H

class Thread{
public:			Thread();
				Thread(bool);
				~Thread();
		void	Spawn(void);
		void	Terminate(void);
virtual	void	main(void);
bool started;
private:
		void	*thread;
unsigned long	threadID;
		enum ThreadStates{
			NOT_STARTED,
			RUNING,
			TERMINATED
		} state;
};

#endif
