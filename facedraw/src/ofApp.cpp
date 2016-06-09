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
	drawcam = false;
	b.init();
	ofBackground(ofColor(20, 20, 20));

	reset_game();
	faceCenter.set(CAM_WIDTH/2.0,CAM_HEIGHT/2.0);
	ofRectMode(OF_RECTMODE_CENTER);

}



void ofApp::draw_world(void) {
	// draw the score and the game world
}

void ofApp::reset_game(void) {
	// draw the score and the game world
	ofApp::drop_faces = 0;
	score = 0;
	paddleSize = 300;
	state.set_state(S_IDLE);
	b.reset();
	cout << "RESET\n";

}

void ofApp::update() {

	cam.update();
	if (cam.isFrameNew()) {
		finder.update(cam);
	}

	if (finder.size() > 0) {
		
		int i = 0;
		faceRect = finder.getObjectSmoothed(i);
		// flip X coordinate
		faceRect.x = ofGetWidth() - faceRect.x - faceRect.width;

		ofPoint fc = faceRect.getCenter();
		// scale center so we can reach the entire screen
		float border = CAM_WIDTH ;
		fc.x = ofMap(fc.x, 0., CAM_WIDTH, -border, CAM_WIDTH + border);
		border = CAM_HEIGHT;
		fc.y = ofMap(fc.y, 0., CAM_HEIGHT, - border,  CAM_HEIGHT + border);
		fc.x -= paddleSize / 2;
		fc.y -= paddleSize / 2;
		paddleRect.set(fc.x,fc.y,paddleSize,paddleSize);

	}

	b.update(state.state);


	// if we got here, we've missed
	if (b.pos.z > Z_NEAR) {
		cout << "MISS!\n";
		b.reset();
	}


	switch (state.state) {
	case S_IDLE:
		if (finder.size()) {
			state.set_state(S_FORWARD);
			drop_faces = 0;
		}
	break;

	case S_FORWARD:
		if (finder.size() == 0) {
			drop_faces++;
			if (drop_faces > DROPPED_FACES_THRESH) {
				drop_faces = 0;
				reset_game();
			}
		}
		else {
			drop_faces = 0;
		}

		// test for collision
		if ((b.pos.z > Z_CLOSE)) {
			if (paddleRect.inside(b.pos.x, b.pos.y)) {
				cout << "Bounce!\n";
				state.set_state(S_BACKWARD);
				b.bounce();
				// WIN!
			}
		}
		break;
	case S_BACKWARD:
		if (b.pos.z < Z_FAR) {
			b.reset();
			score++;
			cout << "WIN!" << "score " << score << "\n";
			state.set_state(S_IDLE);
		}
		break;
	}

}

void ofApp::draw() {

	float spinX = sin(ofGetElapsedTimef()*.35f);
	float spinY = cos(ofGetElapsedTimef()*.075f);

	if (drawcam) {
		ofSetColor(255, 255, 255);//stroke color  
		cam.draw(CAM_WIDTH, 0, -CAM_WIDTH, CAM_HEIGHT);
	}

	// future work, pick biggest face rectangle
	//for (int i = 0; i < finder.size(); i++) {

	if (state.state != S_IDLE) {
		// pick first object

		//ofFill();
		//ofSetColor(255, 255, 255); //fill color  
		//ofRect(x, y, width, height);
		ofNoFill();
		ofSetColor(0, 255, 255);//stroke color  
								//ofRect(x, y, width, height);
		// future change to crosshair method
		//ofDrawRectRounded(faceRect, 50);
		ofPushMatrix();
		ofTranslate(0, 0, Z_CLOSE);
		ofDrawRectRounded(paddleRect,50);
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