#pragma once

#include "ofMain.h"

#define IMG_W 800
#define IMG_H 600

class ofApp : public ofBaseApp{
	public:
		
	void setup();
	void update();
	void draw();

	void updateWave(); // one update of wave equation
	
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
	ofImage U;
	ofImage V;
};
