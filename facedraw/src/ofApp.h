#pragma once

#include "ofMain.h"
#include "ofxCv.h"


// Z positions for ball and goal
#define Z_FAR -3000
#define Z_NEAR -400
#define Z_CLOSE -500

// miss this many frames in a row to drop faces
#define DROPPED_FACES_THRESH 30

class  Ball {
public:
	ofIcoSpherePrimitive isp;
	ofPoint pos, vel;
	// convert sphere to mesh for manipulations
	ofMesh* bmesh;

	int score; //keeps score
	void draw(void);
	void update(int state);
	void init(void);

	void set_velxy(float vx, float vy);
	void set_velz(float vz);
	void bounce(void);
	void reset(void);
	void set_color(ofColor c);
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

	// run once
	void setup();

	// update game coordinates and state
	void update();
	// actually draw the game world
	void draw();
	// draw the game paddle
	void draw_paddle();
	// draw the game world
	void draw_world();
	// start new game
	void reset_game();

	void keyPressed(int key);

	int score; // keep score here
	int drop_faces; // count of frames with no face detected, if above thresh then reset game

	bool drawcam;

	Ball b;

	StateMach state; 

	// center of face as detected by face detection (or mouse)
	ofPoint faceCenter;	
	// size of paddle, centered at faceCenter
	ofRectangle paddleRect;
	// location of face in image
	ofRectangle faceRect;
	float paddleSize;



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
