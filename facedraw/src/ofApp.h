#pragma once

#include "ofMain.h"
#include "ofxCv.h"


// Z positions for ball and goal
#define Z_FAR -3000
#define Z_NEAR -500
#define Z_CLOSE -400

class  Ball {
public:
	ofIcoSpherePrimitive isp;
	ofPoint pos, vel;
	// convert sphere to mesh for manipulations
	ofMesh* bmesh;
	int index;
	void draw(void);
	void update(int state);
	void init(int index);

	void set_velxy(float vx, float vy);
	void set_velz(float vz);
	void bounce(void);
	void reset(void);
};

//cam dimensions
#define CAM_WIDTH 640
#define CAM_HEIGHT 480

// state constants
#define S_IDLE 1000
#define S_FORWARD 1001
#define S_BACKWARD 1002

class StateMach {
	// state machine for game transitions
public:
	int state;
	void set_state(int next_state);
	void print_state(void);
};

class ofApp : public ofBaseApp {
public:


	void setup();
	void update();
	void draw();
	void keyPressed(int key);

	bool drawcam;

	Ball b;

	StateMach state; 

	ofPoint faceCenter;	



	//void keyPressed(int key);
	//void keyReleased(int key);
	//void mouseMoved(int x, int y);
	//void mouseDragged(int x, int y, int button);
	//void mousePressed(int x, int y, int button);
	//void mouseReleased(int x, int y, int button);
	//void mouseEntered(int x, int y);
	//void mouseExited(int x, int y);
	//void windowResized(int w, int h);
	//void dragEvent(ofDragInfo dragInfo);
	//void gotMessage(ofMessage msg);

	ofVideoGrabber cam;
	ofxCv::ObjectFinder finder;
	ofImage sunglasses;
	ofImage saveimg;
	uint16_t id;
};
