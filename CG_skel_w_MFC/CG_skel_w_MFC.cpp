// CG_skel_w_MFC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CG_skel_w_MFC.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/freeglut_ext.h"
#include "vec.h"
#include "mat.h"
#include "InitShader.h"
#include "Scene.h"
#include "Renderer.h"
#include <string>

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

#define MENU_FILE_OPEN 0
#define MENU_MAIN_DEMO 1
#define MENU_MAIN_ABOUT 2
#define MENU_CAMERA_DEFAULT 3
#define MENU_OBJECTS_NONE 4
#define MENU_TRANSFORMS_TRANSLATE 6
#define MENU_TRANSFORMS_ROTATE 7
#define MENU_TRANSFORMS_SCALE 8
#define MENU_TRANSFORMS_SHEAR 9
#define MENU_VIEW_SHOW_VERTEX_NORMALS 10
#define MENU_VIEW_SHOW_FACE_NORMALS 11
#define MENU_VIEW_SHOW_BOUNDING_BOX 12
#define MENU_SHAPES_CUBE 13
#define MENU_OPTIONS_STEP_SIZE 14

// View options
bool isShowVertexNormals = false;
bool isShowFaceNormals = false;
bool isShowBoundingBox = false;

// Rendering variables
Camera selectedCamera;
// TODO: what format am i storing the frame? a simple struct of coordinates? how many? what will be most convinient?
// (Xmin, Ymin, Zmin), (Xmax, Ymax, Zmax)? or do we want to allow a diagonal frame?
mat4 aggregatedSelectedTransforms; // used when applying many transforms simultaneously


Scene *scene;
Renderer *renderer;

int last_x,last_y;
bool lb_down,rb_down,mb_down;

//----------------------------------------------------------------------------
// Callbacks

void display( void )
{
	scene->draw(); // TODO: check that initial black screen is ok
}

void reshape( int width, int height )
{
	// TODO: update the renderer's buffers
}

void keyboard( unsigned char key, int x, int y )
{
	switch (key) {
	case 033:
		exit( EXIT_SUCCESS );
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	//button = {GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON}
	//state = {GLUT_DOWN,GLUT_UP}
	
	//set down flags
	switch(button) 
	{
		case GLUT_LEFT_BUTTON:
			lb_down = (state==GLUT_UP)?0:1;
			break;
		case GLUT_RIGHT_BUTTON:
			rb_down = (state==GLUT_UP)?0:1;
			break;
		case GLUT_MIDDLE_BUTTON:
			mb_down = (state==GLUT_UP)?0:1;	
			break;
		default:
			break;
	}

	// add your code
}

void motion(int x, int y)
{
	// calc difference in mouse movement
	int dx = x - last_x;
	int dy = y - last_y;
	// update last x,y
	last_x = x;
	last_y = y;
}

void fileMenu(int id)
{
	switch (id)
	{
		case MENU_FILE_OPEN:
			CFileDialog dlg(TRUE,_T(".obj"),NULL,NULL,_T("*.obj|*.*"));
			if(dlg.DoModal()==IDOK)
			{
				std::string s((LPCTSTR)dlg.GetPathName());
				scene->loadOBJModel((LPCTSTR)dlg.GetPathName());
			}
			break;
			// TODO: unexpected error
	}
}

void camerasMenu(int id)
{
	// TODO: change camera by the given id
}

void objectsMenu(int id)
{
	// TODO: change the object that is focused by the given id
}

void transformsMenu(int id)
{
	// TODO: change the object that is focused by the given id
}

void viewMenu(int id)
{
	cout << id << endl;
	switch (id)
	{
	case MENU_VIEW_SHOW_VERTEX_NORMALS: // id = 10
		isShowVertexNormals = !isShowVertexNormals;

		if (isShowVertexNormals) 
		{

		}
		else 
		{

		}

		break;
	case MENU_VIEW_SHOW_FACE_NORMALS: // id = 11
		isShowFaceNormals = !isShowFaceNormals;
		// TODO: update the menu items to rename this option with a "<-" postfix
		break;
	case MENU_VIEW_SHOW_BOUNDING_BOX: // id = 12
		isShowBoundingBox = !isShowBoundingBox;
		// TODO: update the menu items to rename this option with a "<-" postfix
		break;
	default: //TODO: error
		break;
	}

	// TODO: re-render the scene
}

void shapesMenu(int id)
{
	// TODO: insert new primitive shape to the scene
}

void optionsMenu(int id)
{
	// TODO: change the object that is focused by the given id
	return; // TODO: what is {id}'s value?
}

void mainMenu(int id)
{
	switch (id)
	{
	case MENU_MAIN_DEMO:
		scene->drawDemo();
		break;
	case MENU_MAIN_ABOUT:
		AfxMessageBox(_T("Computer Graphics"));
		break;
	}
}

void initMenu()
{
	string fileOpen = "Open..";
	string camerasDefaultCamera = "default camera";
	string objectsNone = "None";
	// TODO: continue here


	int openFileMenu = glutCreateMenu(fileMenu);
	glutAddMenuEntry("Open..", MENU_FILE_OPEN);

	int camerasSubMenu = glutCreateMenu(camerasMenu);
	glutAddMenuEntry("default camera", MENU_CAMERA_DEFAULT);
	// TODO: how do i add a list that i update?

	int objectsSubMenu = glutCreateMenu(objectsMenu);
	glutAddMenuEntry("None", MENU_OBJECTS_NONE);
	// TODO: how do i add a list that i update?

	int transformsSubMenu = glutCreateMenu(transformsMenu);
	glutAddMenuEntry("translate", MENU_TRANSFORMS_TRANSLATE);
	glutAddMenuEntry("rotate", MENU_TRANSFORMS_ROTATE);
	glutAddMenuEntry("scale", MENU_TRANSFORMS_SCALE);
	glutAddMenuEntry("shear", MENU_TRANSFORMS_SHEAR);

	int viewSubMenu = glutCreateMenu(viewMenu);
	glutAddMenuEntry("Show vertex normals", MENU_VIEW_SHOW_VERTEX_NORMALS); // TODO: how to make a checkbox?
	glutAddMenuEntry("Show face normals", MENU_VIEW_SHOW_FACE_NORMALS); // TODO: how to make a checkbox?
	glutAddMenuEntry("Show bounding box", MENU_VIEW_SHOW_BOUNDING_BOX); // TODO: how to make a checkbox?
	
	int shapesSubMenu = glutCreateMenu(shapesMenu);
	glutAddMenuEntry("Cube", MENU_SHAPES_CUBE);
	// TODO: how do i add a list that i update?

	int optionsSubMenu = glutCreateMenu(optionsMenu);
	glutAddMenuEntry("Set object step size when moving", MENU_OPTIONS_STEP_SIZE);

	glutCreateMenu(mainMenu);

	glutAddSubMenu("File", openFileMenu);
	glutAddSubMenu("Cameras", camerasSubMenu);
	glutAddSubMenu("Objects", objectsSubMenu);
	glutAddSubMenu("Transforms", transformsSubMenu);
	glutAddSubMenu("View", viewSubMenu);
	glutAddSubMenu("Shapes", shapesSubMenu);
	glutAddSubMenu("Options", optionsSubMenu);
	glutAddMenuEntry("Demo", MENU_MAIN_DEMO);
	glutAddMenuEntry("About", MENU_MAIN_ABOUT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void updateMenu() // TODO: update function signature as necessary
{
	// TODO:
}
//----------------------------------------------------------------------------



int my_main( int argc, char **argv )
{
	//----------------------------------------------------------------------------
	// Initialize window
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA| GLUT_DOUBLE);
	glutInitWindowSize( 512, 512 );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutCreateWindow( "CG" );
	glewExperimental = GL_TRUE;
	glewInit();
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		/*		...*/
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	
	
	renderer = new Renderer(512,512);
	scene = new Scene(renderer);
	//----------------------------------------------------------------------------
	// Initialize Callbacks

	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutMouseFunc( mouse );
	glutMotionFunc ( motion );
	glutReshapeFunc( reshape );
	initMenu();
	

	glutMainLoop();
	delete scene;
	delete renderer;
	return 0;
}

CWinApp theApp;

using namespace std;

int main( int argc, char **argv )
{
	int nRetCode = 0;
	
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		my_main(argc, argv );
	}
	
	return nRetCode;
}
