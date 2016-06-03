#include "ofApp.h"

using namespace ofxCv;
using namespace cv;


void ofApp::setup() {
	ofSetVerticalSync(true);
	ofSetFrameRate(120);
	finder.setup("haarcascade_frontalface_default.xml");
	finder.setPreset(ObjectFinder::Fast);
	finder.getTracker().setSmoothingRate(.3);
	cam.listDevices();
	cam.setDeviceID(0);
	cam.setup(CAM_WIDTH, CAM_HEIGHT);
	//cam.setup(640,480);
	sunglasses.load("sunglasses.png");
	ofEnableAlphaBlending();
	id = 0;
	drawcam = false;
	b.init(0);
	ofBackground(ofColor(20, 20, 20));

	state.set_state(S_IDLE);
	faceCenter.set(CAM_WIDTH/2.0,CAM_HEIGHT/2.0);
}

void ofApp::update() {

	ofRectangle faceRect;

	cam.update();
	if (cam.isFrameNew()) {
		finder.update(cam);
	}


	if (finder.size() > 0) {
		
		int i = 0;
		
		faceRect = finder.getObjectSmoothed(i);
		faceCenter = faceRect.getCenter();
		b.update(state.state);

	}
	else {
		b.update(state.state);
	}

	// if we got here, we've missed
	if (b.pos.z > Z_NEAR) {
		cout << "MISS!\n";
		b.reset();
	}


	switch (state.state) {
	case S_IDLE:
		if (finder.size()) {
			state.set_state(S_FORWARD);
		}
		break;
	case S_FORWARD:
		if (finder.size() == 0) {
			state.set_state(S_IDLE);
		}
		// test for collision
		if ((b.pos.z > Z_CLOSE) || TRUE) {
			if (faceRect.inside(b.pos.x, b.pos.y)) {

				cout << "BOUNCE\n";
				state.set_state(S_BACKWARD);
				b.bounce();
				// WIN!
			}
		}
		break;
	case S_BACKWARD:
		if (b.pos.z < Z_FAR) {
			b.reset();
			state.set_state(S_IDLE);
		}
		break;
	}

}

void ofApp::draw() {
	if (drawcam) {
		ofSetColor(255, 255, 255);//stroke color  
		cam.draw(640, 0, -640, 480);
	}

	// future work, pick biggest face rectangle
	//for (int i = 0; i < finder.size(); i++) {

	if (finder.size() > 0) {
		// pick first object
		ofRectangle facerect = finder.getObjectSmoothed(0);
		facerect.x = ofGetWidth() - facerect.x - facerect.width;
		//ofFill();
		//ofSetColor(255, 255, 255); //fill color  
		//ofRect(x, y, width, height);
		ofNoFill();
		ofSetColor(0, 255, 255);//stroke color  
								//ofRect(x, y, width, height);
		// future change to crosshair method
		ofPushMatrix();
		ofTranslate(0, 0, Z_CLOSE);
		ofDrawRectRounded(facerect, 50);
		ofPopMatrix();
	}


	b.draw();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'p')
	{
		//char name[256];
		//sprintf(name, "img%03d.png", id);
		//saveimg.saveImage(name);
		//id++;
		cout << "P pressed\n";
	}
	if (key == 'f') {
			drawcam = !drawcam;
	}
}



//----------------------------- state machine handlind  -

void StateMach::set_state(int next_state) {
	state = next_state;
	print_state();
}

void StateMach::print_state(void) {
	switch (state) {
	case S_IDLE:
		cout << "State: IDLE\n";
		break;	
	case S_FORWARD:
		cout << "State: FORWARD\n";
		break;
	case S_BACKWARD:
		cout << "State: BACKWARD\n";
		break;
	}
}