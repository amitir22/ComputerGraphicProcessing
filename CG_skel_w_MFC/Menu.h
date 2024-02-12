#include "stdafx.h"

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/freeglut_ext.h"
#include "Camera.h"
#include "MeshModel.h"

using std::cout;
using std::endl;
using std::move;
using std::to_string;

const string PREFIX = "  ";
const string MARKED_PREFIX = "V ";
const string SELECTED_PREFIX = "->";

namespace Menu 
{
	bool isMenuReady = false;
	int menuEntryCounter = 0;

	vector<unique_ptr<Camera>> cameras; // maybe move the entire menu namespace to the skeleton?
	vector<unique_ptr<Model>> models;
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

	int selectedCamera = 0;
	int selectedObject = 0;

	bool isShowVertexNormals = false;
	bool isShowFaceNormals = false;
	bool isShowBoundingBox = false;
	
	void rebuildGlutMenu();
	void buildGlutMenu();
	void bindToGlut();
	void removeFromGlut();

	// menu builders
	void prepareFileMenu();
	void prepareCamerasMenu();
	// TODO: continue

	// menu handlers
	// 
	// TODO: what if i turn all these functions into pointers that will 
	// be set from the outside by assigning?
	void handleMainMenu(int choice);
	void handleFileMenu(int choide);
	void handleCamerasMenu(int choice);
	void handleObjectsMenu(int choice);
	void handleTransformsMenu(int choice);
	void handleViewMenu(int choice);
	void handleShapesMenu(int choice);
	void handleOptionsMenu(int choice);
};
