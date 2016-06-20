#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"


#define CAMW 640
#define CAMH 480

// state constants
#define S_IDLE		1000
#define S_CAPTURE	1001
#define S_SAVE		1002
#define S_INTERIM	1003

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


};
