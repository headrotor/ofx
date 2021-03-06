#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxXmlSettings.h"
#include <signal.h>

//#define RPI
#undef RPI

// for windows (yuck)
#ifndef SIGUSR1
#define SIGUSR1 0
#endif

//cam dimensions
#define CAM_WIDTH 640
#define CAM_HEIGHT 480

//make 1 pixel face movement SCALE_FACE_POS pixels
#define SCALE_FACE_POS 2.0

//screen dimensions
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// Z positions for ball and goal
#define Z_FAR -3000
#define Z_NEAR -400
#define Z_CLOSE -500

#define Z_FIELD -1000
#define Z_FIELD_START -400
#define Z_FIELD_END -1400

// miss this many frames in a row to drop faces
#define DROPPED_FACES_THRESH 30

class  Ball {
public:
	ofIcoSpherePrimitive isp;
	// position and velocity of ball
	ofVec3f pos, vel;
	// convert sphere to mesh for manipulations
	ofMesh* bmesh;

	int score; //keeps score
	void draw(void);
	void update(int state);
	void init(double speed);

	void bounce(ofVec3f spin);
	void reset();
	void set_color(ofColor c);

	ofPoint getCenter();  // get x, y center

	// default Z speeds
	double forwardSpeed; 
	double reverseSpeed;

	ofVec3f goalCenter;
};



// state constants
#define S_IDLE 1000
#define S_FORWARD 1001
#define S_BACKWARD 1002
#define S_CELEBRATE 1003

class StateMach {
	// state machine for game transitions
public:
	int state;
	float start_time;
	float timeout_time; // timeout if in this state >
	void set(int next_state, float timeout);
	void print(void);
	bool timeout(void);
	void reset_timer();
	float time_elapsed();
	float frac_time_elapsed();

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
	void draw_world_old();
	// start new game
	void reset_game();
	// clean exit (don't hang camera!) on SIGUSR1
	static void clean_exit(int signal);


	// flags for testing behaviors
	// use face velocity to control ball direction
	bool vel_flag = false;


	// precompute goal center
	ofVec3f goalCenter;
	
	// update the celebration animation
	void update_celebrate(int win);
	int celebrate; 

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

	ofRectangle goalRect;
	// location of goal

	ofRectangle faceRect;

	float paddleSize;

	vector<string> arguments;

	// font for scores, etc
	ofTrueTypeFont font;
	char score_str[24];



	// field 
	ofMesh field;

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
	ofxXmlSettings config;

	// config settings
	int dropped_faces_thresh;
	int cannyPruning;
	int frameRate;
	double smoothingRate;
	double forwardSpeed;
	double reverseSpeed;
	double english; // how much off-center paddle affects ball target; 
	// english = 0 -> always hit center of target

};
