#include "ofApp.h"

int main(int argc, char *argv[]) {

#ifdef RPI
	ofSetupOpenGL(SCREEN_WIDTH, SCREEN_HEIGHT, OF_WINDOW);
#else
	//ofSetupOpenGL(SCREEN_WIDTH, SCREEN_HEIGHT, OF_GAME_MODE);
	ofSetupOpenGL(SCREEN_WIDTH, SCREEN_HEIGHT, OF_WINDOW);
#endif
	ofApp *app = new ofApp();

	app->arguments = vector<string>(argv, argv + argc);
	ofRunApp(app);
}
