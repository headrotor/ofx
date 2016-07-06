#include "ofApp.h"

int main(int argc, char *argv[]) {
	cout << "got n args " << argc;
	//ofSetupOpenGL(640, 480, OF_WINDOW);
	//ofSetupOpenGL(640, 480, OF_FULLSCREEN);
	ofSetupOpenGL(640, 480, OF_GAME_MODE);
	ofApp *app = new ofApp();

	app->arguments = vector<string>(argv, argv + argc);
	ofRunApp(app);
}
