
#ifdef OPENGL

#include <posix/stdio.h>
//#include <GL/glut.h>
#include <Interface/Window.h>
#include <Interface/Workspace.h>

static Workspace *__workspace;

Workspace::Workspace(){
/*
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitWindowSize(800,600);
	glutInit(0,"");
	glutCreateWindow("OpenGL");
	//init(argc == 1 ? "data/fendi.rgb" : argv[1]);
	//glutDisplayFunc(display);
	//glutKeyboardFunc(key);
	//glutSpecialFunc(special);
	//glutReshapeFunc(reshape);
	glutMainLoop();
*/
}

Workspace *Workspace::GetActiveWorkspace(){
	if(!__workspace){
		printf("Initializing worspaces.\n");
		__workspace = new Workspace();
	}
	return __workspace;
}

#endif

