#pragma once

#include "ofMain.h"
#include "ofxCv.h"

#define NUM 10

class  Ball {
public:
	ofIcoSpherePrimitive isp;
	ofPoint pos, vel;
	float z;
	// convert sphere to mesh for manipulations
	ofMesh* bmesh;
	int index;
	void draw(void);
	void update(float x, float y, bool face);
	void init(int index);

};


class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);


	Ball b[NUM];

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
