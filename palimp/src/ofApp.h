#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"


#define CAMW 640
#define CAMH 480

// state constants
#define S_IDLE		1000 // no face recognized
#define S_HELLO	    1001 // face recognized
#define S_CAPTURE	1002 // face recognized, show greeting
#define S_YES_IMG	1003 // save image, show thanks
#define S_NO_IMG	1004 // don't save image, show thanks

class StateMach {
	// state machine for game transitions
public:
	int state;
	float start_time;
	float timeout_time; // timeout if in this state >
	void set(int next_state);
	void print(void);
	bool timeout(void);
	void reset_timer();
	float time_elapsed();

};

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);

	ofVideoGrabber cam;
	ofxCv::ObjectFinder finder;
	//ofImage sunglasses;
	ofImage saveimg;
	uint16_t id;
	ofImage grabimg;
	ofImage cropimg;

	StateMach state;

	ofxCvColorImage 	colorimg;
	ofxCvGrayscaleImage 	grayimg;

	ofTrueTypeFont msg;

};
