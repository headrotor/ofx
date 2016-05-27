#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"

#define CROPX 200
#define CROPY 30
#define CROPW 100
#define CROPH 100

#define CAMW 640
#define CAMH 480

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


	ofxCvColorImage 	colorimg;
	ofxCvGrayscaleImage 	grayimg;
	ofxCvGrayscaleImage 	cropdiff;
	ofRectangle cropr;
};
