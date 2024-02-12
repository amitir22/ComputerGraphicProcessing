#include "stdafx.h"

#include "Menu.h"

void Menu::rebuildGlutMenu()
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
	
	if (selectedCamera == 0)
		currentCameraNameWprefix = SELECTED_PREFIX + currentCameraName;
	
	glutAddMenuEntry(currentCameraNameWprefix.c_str(), menuEntryCounter++);
	
	for (int cameraIndex = 1; cameraIndex < cameras.size(); cameraIndex++)
	{
		currentCameraName = cameraPrefix + to_string(cameraIndex);
		currentCameraNameWprefix = PREFIX + currentCameraName;

		if (selectedCamera == cameraIndex)
			currentCameraNameWprefix = SELECTED_PREFIX + currentCameraName;

		glutAddMenuEntry(currentCameraNameWprefix.c_str(), menuEntryCounter++);
	}

	// objects menu
	string objectPrefix = "object ";
	string currentObjectName = "None";
	string currentObjectNameWprefix = PREFIX + currentObjectName;
	objectsSubMenuID = glutCreateMenu(handleObjectsMenu);

	if (selectedCamera == 0)
		currentObjectNameWprefix = SELECTED_PREFIX + currentObjectName;

	glutAddMenuEntry(currentObjectName.c_str(), menuEntryCounter++);

	for (int objectIndex = 1; objectIndex < cameras.size(); objectIndex++)
	{
		currentObjectName = objectPrefix + to_string(objectIndex);
		currentObjectNameWprefix = PREFIX + currentObjectName;

		if (selectedObject == objectIndex)
			currentObjectNameWprefix = SELECTED_PREFIX + currentObjectName;

		glutAddMenuEntry(currentObjectName.c_str(), menuEntryCounter++);
	}

	// transforms menu
	transformsSubMenuID = glutCreateMenu(handleTransformsMenu);
	glutAddMenuEntry("translate", menuEntryCounter++);
	glutAddMenuEntry("rotate", menuEntryCounter++);
	glutAddMenuEntry("scale", menuEntryCounter++);

	// view menu
	string viewVertexNormalsPrefix = PREFIX;
	string viewFaceNormalsPrefix = PREFIX;
	string viewBoundingBoxPrefix = PREFIX;

	if (isShowVertexNormals) {
		viewVertexNormalsPrefix = MARKED_PREFIX;
	}
	if (isShowFaceNormals) {
		viewFaceNormalsPrefix = MARKED_PREFIX;
	}
	if (isShowBoundingBox) {
		viewBoundingBoxPrefix = MARKED_PREFIX;
	}

	string viewVertexNormals = viewVertexNormalsPrefix + string("Show vertex normals");
	string viewFaceNormals = viewFaceNormalsPrefix + string("Show face normals");
	string viewBoundingBox = viewBoundingBoxPrefix + string("Show bounding box");

	viewSubMenuID = glutCreateMenu(handleViewMenu);
	glutAddMenuEntry(viewVertexNormals.c_str(), menuEntryCounter++);
	glutAddMenuEntry(viewFaceNormals.c_str(), menuEntryCounter++);
	glutAddMenuEntry(viewBoundingBox.c_str(), menuEntryCounter++);
	
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
	// TODO: delete each menu item
	glutDestroyMenu(mainMenuID);
}

void Menu::handleMainMenu(int choice)
{
	if (choice == demoMenuEntryID)
		return; // TODO
	else if (choice == aboutMenuEntryID)
		AfxMessageBox(_T("Computer Graphics"));
}

void Menu::handleFileMenu(int choice)
{
	if (choice == fileOpenMenuEntryID)
	{
		CFileDialog dlg(TRUE, _T(".obj"), NULL, NULL, _T("*.obj|*.*"));
		if (dlg.DoModal() == IDOK)
		{
			std::string s((LPCTSTR)dlg.GetPathName());
			//scene->loadOBJModel((LPCTSTR)dlg.GetPathName()); TODO: how?
		}
	}
	/*
	switch (choice)
	{
	case MENU_FILE_OPEN:
		CFileDialog dlg(TRUE, _T(".obj"), NULL, NULL, _T("*.obj|*.*"));
		if (dlg.DoModal() == IDOK)
		{
			std::string s((LPCTSTR)dlg.GetPathName());
			scene->loadOBJModel((LPCTSTR)dlg.GetPathName());
		}
		break;
		// TODO: unexpected error
	}*/
}

void Menu::handleCamerasMenu(int choice)
{

}

void Menu::handleObjectsMenu(int choice)
{

}

void Menu::handleTransformsMenu(int choice)
{

}

void Menu::handleViewMenu(int choice)
{

}

void Menu::handleShapesMenu(int choice)
{

}

void Menu::handleOptionsMenu(int choice)
{

}