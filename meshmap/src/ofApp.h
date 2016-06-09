#pragma once

#include "ofMain.h"
#include "ofxCv.h"

#define IMG_W 800
#define IMG_H 600

#define DEPTH 3

class Goop {
	// for wave equation, reaction-diffusion, etc...
public:
	int h, w;
	int pntr;

	ofImage U;
	ofImage V;

	ofFloatPixels fp[DEPTH];
	void setup(ofImage img);
	void updateWave(ofImage force);

};

class ofApp : public ofBaseApp{
	public:
		
	ofVideoGrabber cam;
	ofxCv::ObjectFinder finder;
	ofRectangle faceRect;
	// center of face
	ofPoint fc;

	void setup();
	void update();
	void draw();

	//int toggle;
	//void updateWave(); // one update of wave equation
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

    ofShader shader;
    ofPlanePrimitive plane;
	ofImage img;
	//ofImage U;
	//ofImage V;

	Goop goop;

	//ofFloatPixels Up;
	//ofFloatPixels Vp;
	//ofFloatPixels Wp;

	//ofFloatPixels fp[DEPTH];
};

