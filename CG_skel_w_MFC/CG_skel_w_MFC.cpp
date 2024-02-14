// CG_skel_w_MFC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <chrono>
#include "CG_skel_w_MFC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object
#include <string>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/freeglut_ext.h"
#include "vec.h"
#include "mat.h"
#include "InitShader.h"
#include "Scene.h"
#include "Renderer.h"
#include "Camera.h"
#include "constants.h"
#include "Geometry.h"
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

using namespace std;

// Menu options
const string PREFIX = "  ";
const string MARKED_PREFIX = "V ";
const string SELECTED_PREFIX = "->";

bool isMenuReady = false;
int menuEntryCounter = 0;

int mainMenuID;
int fileSubMenuID;
int fileOpenMenuEntryID;
int camerasSubMenuID;
int objectsSubMenuID;
int transformsSubMenuID;
int viewSubMenuID;
int shapesSubMenuID;
int optionsSubMenuID;
int demoMenuEntryID;
int aboutMenuEntryID;

int camerasStartOffsetID; 
int camerasAddCameraEntryID;
int objectsStartOffsetID;
int viewShowVertexNormalsMenuEntryID;
int viewShowFaceNormalsMenuEntryID;
int viewShowBoundingBoxMenuEntryID;
int transformsTranslateEntryID;
int transformsRotateEntryID;
int transformsScaleEntryID;
int transformsAdvancedEntryID;


// Rendering variables
Camera selectedCamera;
// TODO: what format am i storing the frame? a simple struct of coordinates? how many? what will be most convinient?
// (Xmin, Ymin, Zmin), (Xmax, Ymax, Zmax)? or do we want to allow a diagonal frame?
mat4 aggregatedSelectedTransforms; // used when applying many transforms simultaneously


Scene *scene;
Renderer *renderer;

// Mouse movement
float lastX = CG::DEFAULT_WIDTH / 2;
float lastY = CG::DEFAULT_HEIGHT / 2;
bool firstMouse = true;
bool lb_down, rb_down, mb_down;

//----------------------------------------------------------------------------
// Callbacks

void display( void )
{
	scene->draw(); // TODO: check that initial black screen is ok
}

void reshape( int width, int height )
{
	// Set the viewport to cover the whole window
	glViewport(0, 0, width, height);
	if (height == 0) height = 1;

	// Tell your Renderer to handle window reshape
	renderer->handleWindowReshape(width, height);
	scene->handleWindowReshape(width, height);
}

void keyboard( unsigned char key, int x, int y )
{
	Camera* activeCamera = scene->getActiveCamera();
	bool shouldRedraw = false;
	switch (key) {
	case 033: // Esc Key
		exit( EXIT_SUCCESS );
		break;
	// if someone pressed WASD, send it to camera to HandlekeyboardInput()
	case 'w':
		activeCamera->handleKeyboardInput(Camera_Movement::FORWARD, 0.1);
		shouldRedraw = true;
		break;
	case 's':
		activeCamera->handleKeyboardInput(Camera_Movement::BACKWARD, 0.1);
		shouldRedraw = true;
		break;
	case 'a':
		activeCamera->handleKeyboardInput(Camera_Movement::LEFT, 0.1);
		shouldRedraw = true;
		break;
	case 'd':
		activeCamera->handleKeyboardInput(Camera_Movement::RIGHT, 0.1);
		shouldRedraw = true;
		break;
	case 'q':
		activeCamera->handleKeyboardInput(Camera_Movement::UP, 0.1);
		shouldRedraw = true;
		break;
	case 'e':
		activeCamera->handleKeyboardInput(Camera_Movement::DOWN, 0.1);
		shouldRedraw = true;
		break;
	}
	if (shouldRedraw) {
		glutPostRedisplay();
	}
}

void mouseWheel(int button, int dir, int x, int y) {
	if (dir > 0) {
		cout << "scroll up" << endl;
		mat4 scaleMatrix =  Geometry::makeScaleMatrix(vec3(1.1, 1.1, 1.1));
		scene->applyTransformation(scaleMatrix);
	}
	else {
		cout << "scroll down" << endl;
		mat4 scaleMatrix =  Geometry::makeScaleMatrix(vec3(0.9, 0.9, 0.9));
		scene->applyTransformation(scaleMatrix);
	}

	return;
}

// Handle mouse press and release events
void mouse(int button, int state, int x, int y)
{
	// Nice drag idea https://stackoverflow.com/a/12984457/4399305
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

// Handle mouse movements
void motion(int x, int y)
{
	Camera* activeCamera = scene->getActiveCamera();
	if (firstMouse)
	{
		lastX = x;
		lastY = y;
		firstMouse = false;
	}
	// calc difference in mouse movement
	float dx = x - lastX;
	float dy = lastY - y;
	// update last x,y
	lastX = x;
	lastY = y;
	activeCamera->handleMouseMovement(dx, dy);
}

// Menu handlers

void refreshGUI(bool shouldUpdateMenu/* = true */)
{
	// TODO: delete
	cout << "refreshGUI(" << shouldUpdateMenu << ")" << endl;
	if (shouldUpdateMenu)
		Menu::updateGlutMenu();
	scene->draw();
}

void handleFileMenu(int id)
{
	// TODO: delete
	cout << "handleFileMenu(" << id << ")" << endl;
	if (id == fileOpenMenuEntryID)
	{
		CFileDialog dlg(TRUE, _T(".obj"), NULL, NULL, _T("*.obj|*.*"));
		if (dlg.DoModal() == IDOK)
		{
			std::string s((LPCTSTR)dlg.GetPathName());
			scene->loadOBJModel((LPCTSTR)dlg.GetPathName());
		}
	}
}

void handleCamerasMenu(int id)
{
	// TODO: delete
	cout << "handleCamerasMenu(" << id << ")" << endl;
	scene->activeCamera = id - camerasStartOffsetID;
	// TODO: delete
	cout << "activeCamera = " << scene->activeCamera << endl;
}

void handleObjectsMenu(int id) // TODO: consider renaming all objects to models
{
	// TODO: delete
	cout << "handleObjectsMenu(" << id << ")" << endl;
	scene->activeModel = id - objectsStartOffsetID;
	// TODO: delete
	cout << "activeModel = " << scene->activeModel << endl;
}

void handleTransformsMenu(int id)
{
	// TODO: delete
	cout << "handleTransformsMenu(" << id << ")" << endl;

	if (id == transformsScaleEntryID)
	{
		vec3 scaleVec = promptScaleShell();

		scene->models[scene->activeModel]->scale(scaleVec);
	}
	else if (id == transformsTranslateEntryID)
	{
		vec3 translateVec = promptTranslateShell();

		scene->models[scene->activeModel]->translate(translateVec);
	}
	else if (id == transformsRotateEntryID)
	{
		vec4 temp = promptRotateShell();
		vec3 axisVec = { temp[0], temp[1], temp[2] };
		float angle = temp[3];

		scene->models[scene->activeModel]->rotate(axisVec, angle);
	}
	//else if (id == transformsAdvancedEntryID) // unavailable for now
	//{
	//
	//}

	refreshGUI();
}

void handleViewMenu(int id)
{
	// TODO: delete
	cout << "handleViewMenu(" << id << ")" << endl;

	if (viewShowVertexNormalsMenuEntryID == id)
	{
		scene->isShowVertexNormals = !(scene->isShowVertexNormals);
	}
	else if (viewShowFaceNormalsMenuEntryID == id)
	{
		scene->isShowFaceNormals = !(scene->isShowFaceNormals);
	}
	else if (viewShowBoundingBoxMenuEntryID == id)
	{
		scene->isShowBoundingBox = !(scene->isShowBoundingBox);
	}

	refreshGUI();
}

void handleShapesMenu(int id)
{
	// TODO: delete
	cout << "handleShapesMenu(" << id << ")" << endl;
	// TODO: insert new primitive shape to the scene
}

void handleOptionsMenu(int id)
{
	// TODO: delete
	cout << "handleOptionsMenu(" << id << ")" << endl;
	// TODO: 
}

void handleMainMenu(int id)
{
	// TODO: delete
	cout << "handleMainMenu(" << id << ")" << endl;
	if (id == demoMenuEntryID)
		scene->drawDemo();
	else if (id == aboutMenuEntryID)
		AfxMessageBox(_T("Computer Graphics"));
}

void initMenu()
{
	// TODO: delete
	cout << "initMenu()" << endl;
	Menu::updateGlutMenu();
}

// Shell handling:

vec3 promptScaleShell()
{
	vec3 scale;

	// TODO: delete
	cout << "promptScaleShell()" << endl;

	cout << "scale x = ";
	cin >> scale.x;
	cout << "scale y = ";
	cin >> scale.y;
	cout << "scale z = ";
	cin >> scale.z;

	// TODO: delete
	cout << "scale = (" << scale.x << ", " << scale.y << ", " << scale.z << ")" << endl;

	return scale;
}
vec3 promptTranslateShell()
{
	vec3 translate;

	// TODO: delete
	cout << "promptTranslateShell()" << endl;

	cout << "translate x = ";
	cin >> translate.x;
	cout << "translate y = ";
	cin >> translate.y;
	cout << "translate z = ";
	cin >> translate.z;

	// TODO: delete
	cout << "translate = (" << translate.x << ", " << translate.y << ", " << translate.z << ")" << endl;

	return translate;
}
vec4 promptRotateShell()
{
	vec4 rotate;

	// TODO: delete
	cout << "promptRotateShell()" << endl;

	cout << "axis x = ";
	cin >> rotate.x;
	cout << "axis y = ";
	cin >> rotate.y;
	cout << "axis z = ";
	cin >> rotate.z;
	cout << "angle = ";
	cin >> rotate.w;

	// TODO: delete
	cout << "axis = (" << rotate.x << ", " << rotate.y << ", " << rotate.z << ")" << endl;
	cout << "angle = " << rotate.w;

	return rotate;
}

// Menu handling:

void Menu::updateGlutMenu()
{
	if (isMenuReady)
	{
		removeFromGlut();
	}

	menuEntryCounter = 0;
	buildGlutMenu();
	bindToGlut();
}

void Menu::buildGlutMenu()
{
	// file menu
	fileSubMenuID = glutCreateMenu(handleFileMenu);
	fileOpenMenuEntryID = menuEntryCounter++;
	glutAddMenuEntry("Open..", fileOpenMenuEntryID);

	// cameras menu
	string cameraPrefix = "camera ";
	string currentCameraName = "default camera";
	string currentCameraNameWprefix = PREFIX + currentCameraName;
	camerasSubMenuID = glutCreateMenu(handleCamerasMenu);

	if (scene->activeCamera == 0) // special case = default
		currentCameraNameWprefix = SELECTED_PREFIX + currentCameraName;
	camerasStartOffsetID = menuEntryCounter++;
	glutAddMenuEntry(currentCameraNameWprefix.c_str(), camerasStartOffsetID);

	for (int cameraIndex = 1; cameraIndex < scene->cameras.size(); cameraIndex++)
	{
		currentCameraName = cameraPrefix + to_string(cameraIndex);
		currentCameraNameWprefix = PREFIX + currentCameraName;

		if (scene->activeCamera == cameraIndex)
			currentCameraNameWprefix = SELECTED_PREFIX + currentCameraName;

		glutAddMenuEntry(currentCameraNameWprefix.c_str(), menuEntryCounter++);
	}

	camerasAddCameraEntryID = menuEntryCounter++;
	glutAddMenuEntry("+ make dup", camerasAddCameraEntryID);

	// objects menu
	string objectPrefix = "object ";
	string currentObjectName = "None";
	string currentObjectNameWprefix = PREFIX + currentObjectName;
	objectsSubMenuID = glutCreateMenu(handleObjectsMenu);

	if (scene->activeModel == 0) // special case = default
		currentObjectNameWprefix = SELECTED_PREFIX + currentObjectName;
	objectsStartOffsetID = menuEntryCounter++;
	glutAddMenuEntry(currentObjectName.c_str(), objectsStartOffsetID);

	for (int objectIndex = 1; objectIndex < scene->models.size(); objectIndex++)
	{
		currentObjectName = objectPrefix + to_string(objectIndex);
		currentObjectNameWprefix = PREFIX + currentObjectName;

		if (scene->activeModel == objectIndex)
			currentObjectNameWprefix = SELECTED_PREFIX + currentObjectName;

		glutAddMenuEntry(currentObjectName.c_str(), menuEntryCounter++);
	}

	// transforms menu
	transformsSubMenuID = glutCreateMenu(handleTransformsMenu);
	transformsTranslateEntryID = menuEntryCounter++;
	glutAddMenuEntry("translate", transformsTranslateEntryID);
	transformsRotateEntryID = menuEntryCounter++;
	glutAddMenuEntry("rotate", transformsRotateEntryID);
	transformsScaleEntryID = menuEntryCounter++;
	glutAddMenuEntry("scale", transformsScaleEntryID);
	//transformsAdvancedEntryID = menuEntryCounter++;
	//glutAddMenuEntry("advanced", transformsAdvancedEntryID);

	// view menu
	string viewVertexNormalsPrefix = PREFIX;
	string viewFaceNormalsPrefix = PREFIX;
	string viewBoundingBoxPrefix = PREFIX;

	if (scene->isShowVertexNormals) {
		viewVertexNormalsPrefix = MARKED_PREFIX;
	}
	if (scene->isShowFaceNormals) {
		viewFaceNormalsPrefix = MARKED_PREFIX;
	}
	if (scene->isShowBoundingBox) {
		viewBoundingBoxPrefix = MARKED_PREFIX;
	}

	string viewVertexNormals = viewVertexNormalsPrefix + string("Show vertex normals");
	string viewFaceNormals = viewFaceNormalsPrefix + string("Show face normals");
	string viewBoundingBox = viewBoundingBoxPrefix + string("Show bounding box");

	viewSubMenuID = glutCreateMenu(handleViewMenu);
	viewShowVertexNormalsMenuEntryID = menuEntryCounter++;
	glutAddMenuEntry(viewVertexNormals.c_str(), viewShowVertexNormalsMenuEntryID);
	viewShowFaceNormalsMenuEntryID = menuEntryCounter++;
	glutAddMenuEntry(viewFaceNormals.c_str(), viewShowFaceNormalsMenuEntryID);
	viewShowBoundingBoxMenuEntryID = menuEntryCounter++;
	glutAddMenuEntry(viewBoundingBox.c_str(), viewShowBoundingBoxMenuEntryID);

	// shapes menu
	shapesSubMenuID = glutCreateMenu(handleShapesMenu);
	glutAddMenuEntry("Cube", menuEntryCounter++);

	// options menu
	optionsSubMenuID = glutCreateMenu(handleOptionsMenu);
	glutAddMenuEntry("Set object step size when moving", menuEntryCounter++);

	// main menu
	mainMenuID = glutCreateMenu(handleMainMenu);

	glutAddSubMenu("File", fileSubMenuID);
	glutAddSubMenu("Cameras", camerasSubMenuID);
	glutAddSubMenu("Objects", objectsSubMenuID);
	glutAddSubMenu("Transforms", transformsSubMenuID);
	glutAddSubMenu("View", viewSubMenuID);
	glutAddSubMenu("Shapes", shapesSubMenuID);
	glutAddSubMenu("Options", optionsSubMenuID);

	demoMenuEntryID = menuEntryCounter++;
	glutAddMenuEntry("Demo", demoMenuEntryID);

	aboutMenuEntryID = menuEntryCounter++;
	glutAddMenuEntry("About", aboutMenuEntryID);
}

void Menu::bindToGlut()
{
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void Menu::removeFromGlut()
{
	// TODO: delete
	cout << "removeFromGlut() | menuEntryCounter = " << menuEntryCounter << endl;
	// free menu entries
	// TODO: make sure this loop doesn't run out of bounds.
	for (int i = 0; i < menuEntryCounter; ++i) {
		glutRemoveMenuItem(i); // Remove each menu item
	}

	// free submenus
	glutDestroyMenu(fileSubMenuID);
	glutDestroyMenu(camerasSubMenuID);
	glutDestroyMenu(objectsSubMenuID);
	glutDestroyMenu(transformsSubMenuID);
	glutDestroyMenu(viewSubMenuID);
	glutDestroyMenu(shapesSubMenuID);
	glutDestroyMenu(optionsSubMenuID);

	// free main menu
	glutDestroyMenu(mainMenuID);
}


//----------------------------------------------------------------------------


int my_main( int argc, char **argv )
{
	//----------------------------------------------------------------------------
	// Initialize window
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA| GLUT_DOUBLE);
	glutInitWindowSize( CG::DEFAULT_WIDTH, CG::DEFAULT_HEIGHT);
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	int glutGetWindow(void);
	glutCreateWindow( "CG" );
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		/*		...*/
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));


	
	renderer = new Renderer(CG::DEFAULT_WIDTH,CG::DEFAULT_HEIGHT);
	scene = new Scene(renderer);
	//----------------------------------------------------------------------------
	// Initialize Callbacks

	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	//glutMouseFunc( mouse );
	glutMouseWheelFunc(mouseWheel);
	glutMotionFunc ( motion );
	glutReshapeFunc( reshape );
	initMenu();
	

	glutMainLoop();
	delete scene;
	delete renderer;
	return 0;
}

CWinApp theApp; // This is unused

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
