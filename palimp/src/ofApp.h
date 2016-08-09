#pragma once

#include <signal.h>
#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"

//#define RPI true

#define CAM_WIDTH 640
#define CAM_HEIGHT 480

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// defaults for config settings
#define NUM_IMAGES  1
#define FACE_DROPPED_THRESHOLD 30
#define NOD_THRESHOLD 25.

// state constants
#define S_IDLE		1000 // no face recognized
#define S_HELLO	    1001 // face recognized
#define S_QUESTION	1002 // display question
#define S_THREE	    1003 // display question
#define S_TWO	    1004 // display question
#define S_ONE	    1005 // display question
#define S_CAPTURE	1006 // face recognized, show greeting
#define S_YES_COUNT 1007 // count down to image capture
#define S_YES_IMG	1008 // save image, show thanks
#define S_NO_IMG	1009 // don't save image, show thanks

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
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void store_image();
	static void clean_exit(int signal);
	vector<string> arguments;
    // idle processing
	void init_idle();
	void draw_idle();
	void draw_idle1();
	void draw_idle2();


	ofImage idle_image;
	//ofDirectory imgs;
	vector <ofImage> gray_images;
	vector <ofRectangle> gray_rects;
	ofRectangle parse_path(string path);

	bool store_flag = false; // store only one image per face detected
	bool yes_flag = false; // show recent face in idle if yes

	ofVideoGrabber cam;
	ofxCv::ObjectFinder finder;
	//ofImage sunglasses;
	ofImage saveimg;
	uint16_t id;
	ofImage grabimg;
	ofImage cropimg;
	ofRectangle facerect;
	ofRectangle saverect;

	StateMach state;

	ofxCvColorImage 	colorimg;
	ofxCvGrayscaleImage 	grayimg;

	ofTrueTypeFont msg;

	// running average of x and y face velocities
	// to detect nod yes or no

	float avg_yvel;
	float avg_xvel;

	// scaling to convert between screen and cam rect sizes
	float xscale = float(SCREEN_WIDTH) / float(CAM_WIDTH);
	float yscale = float(SCREEN_HEIGHT) / float(CAM_HEIGHT);


	ofxXmlSettings config;

	// config settings follow
	int face_dropped_threshold;
	double nod_threshold;
	// number of images to overlap
	int num_images;
	
};
