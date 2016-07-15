#include "ofApp.h"
#include <signal.h>

//void handler(int signal) {
//	exit(0); //this makes it to exit in a "clean" way  
//}

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
