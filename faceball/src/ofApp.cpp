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
	//sunglasses.load("sunglasses.png");
	ofEnableAlphaBlending();
	drawcam = false;
	b.init();
	ofBackground(ofColor(20, 20, 20));

	reset_game();
	faceCenter.set(CAM_WIDTH / 2.0, CAM_HEIGHT / 2.0);
	ofRectMode(OF_RECTMODE_CENTER);


	// set up font  
	//font.loadFont("verdana.ttf", 36, true, false, true, 0.1);
	font.loadFont("Hyperspace.otf", 36, true, false, true, 0.1);

	// set up field
	/*field.addVertex(ofVec3f(20, 20));
	field.addColor(ofColor::red);
	field.addVertex(ofVec3f(40, 20));
	field.addColor(ofColor::red);
	field.addVertex(ofVec3f(40, 40));
	field.addColor(ofColor::red);
	field.addVertex(ofVec3f(20, 40));
	field.addColor(ofColor::red);
	field.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
*/


	field = field.plane(1000, 500, 8, 4);
	//field.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
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
		float border = CAM_WIDTH;
		fc.x = ofMap(fc.x, 0., CAM_WIDTH, -border, CAM_WIDTH + border);
		border = CAM_HEIGHT;
		fc.y = ofMap(fc.y, 0., CAM_HEIGHT, -border, CAM_HEIGHT + border);
		fc.x -= paddleSize / 2;
		fc.y -= paddleSize / 2;
		paddleRect.set(fc.x, fc.y, paddleSize, paddleSize);

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
			else {
				state.set_state(S_IDLE);
			}

		}
		break;
	case S_BACKWARD:
		if (b.pos.z < Z_FAR) {
			b.reset();
			celebrate = Z_FAR/2;
			score++;
			cout << "WIN!" << "score " << score << "\n";
			state.set_state(S_CELEBRATE);
		}
		break;
	case S_CELEBRATE:
		if (celebrate > 300) {
			b.reset();
			state.set_state(S_IDLE);
		}
		break;

	}

}
void ofApp::draw_world(void) {
	// draw the score and the game world


	// draw field grid


	//draw score
	ofPushMatrix();
	ofTranslate(0, 0, 0);
	ofSetColor(0, 255, 0);//stroke color 
	sprintf(score_str, "SCORE %02d", score);
	font.drawString(score_str, ofGetWidth() - 200, 30);
	//font.drawStringAsShapes("test", ofGetWidth() / 2 + 20, ofGetHeight() / 2);
	ofPopMatrix();


	ofPushMatrix();
	ofTranslate(ofGetWidth() / 2., 0, Z_FAR / 2);
	//ofTranslate(ofGetWidth() / 2., ofGetHeight() / 2., 30);
	ofRotateX(90.);
	field.drawWireframe();
	ofPopMatrix();



}


void ofApp::update_celebrate(int win) {

	celebrate += 10;

	ofPushMatrix();
	//ofTranslate(ofGetWidth() / 2., ofGetHeight() / 2., Z_FAR);


	ofTranslate(ofGetWidth() / 2., ofGetHeight() / 2., celebrate);
	if (win) {
		ofRotateX(float(celebrate));
		ofRectangle rect = font.getStringBoundingBox("GOAL!!!", 0, 0);
		font.drawStringAsShapes("GOAL!!!", -rect.width/2, 0);

	}
	else {
		font.drawStringAsShapes("MISS", 0, 0);
	}
	//field.drawWireframe();
	ofPopMatrix();

}


void ofApp::draw() {
	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 10, 10);
	float spinX = sin(ofGetElapsedTimef()*.35f);
	float spinY = cos(ofGetElapsedTimef()*.075f);

	if (drawcam) {
		ofSetColor(255, 255, 255);//stroke color  
		cam.draw(CAM_WIDTH, 0, -CAM_WIDTH, CAM_HEIGHT);
	}

	draw_world();
	// future work, pick biggest face rectangle
	//for (int i = 0; i < finder.size(); i++) {

	// draw paddle
	ofSetColor(0, 255, 255);//stroke color  
							//ofRect(x, y, width, height);
							// future change to crosshair method
							//ofDrawRectRounded(faceRect, 50);
	ofNoFill();
	ofPushMatrix();
	ofTranslate(0, 0, Z_CLOSE);
	ofDrawRectRounded(paddleRect, 50);
	ofPopMatrix();

	switch (state.state) {
	case S_FORWARD:
	case S_BACKWARD:
	case S_IDLE:
		b.draw();
		break;
	case S_CELEBRATE:
		update_celebrate(1);
		break;
	}


}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case 'c':
		drawcam = !drawcam;
		break;

	case 'f':
		ofSetFullscreen(true);
		break;

	case 'n':
		ofSetFullscreen(false);
		break;

	case 'x':
		ofExit();

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
	case S_CELEBRATE:
		cout << "State: CELEBRATE\n";
		break;
	}
}