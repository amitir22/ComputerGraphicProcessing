#pragma once

#include "resource.h"

void display( void );
void reshape( int width, int height );
void keyboard( unsigned char key, int x, int y );
void mouse(int button, int state, int x, int y);

void refreshGUI(bool shouldUpdateMenu = true);

void handleFileMenu(int id);
void handleCamerasMenu(int id);
void handleObjectsMenu(int id);
void handleTransformsMenu(int id);
void handleViewMenu(int id);
void handleShapesMenu(int id);
void handleOptionsMenu(int id);
void handleMainMenu(int id);
void initMenu();

namespace Menu
{
	void updateGlutMenu();
	void buildGlutMenu();
	void bindToGlut();
	void removeFromGlut();
};
