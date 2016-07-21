#include "ofApp.h"


using namespace ofxCv;
using namespace cv;


void ofApp::setup() {
	ofSetVerticalSync(true);
	//ofSetFrameRate(120);
	ofSetFrameRate(60);
	finder.setup("haarcascade_frontalface_default.xml");
	finder.setPreset(ObjectFinder::Fast);
	finder.setFindBiggestObject(true);
	//finder.getTracker().setSmoothingRate(.3);
	// samller = smoother
	finder.getTracker().setSmoothingRate(0.2);
	// ignore low-contrast regions
	finder.setCannyPruning(true);
	//cam.listDevices();
	cam.setDeviceID(0);
	cam.setup(CAM_WIDTH, CAM_HEIGHT);
	//cam.setup(640,480);
	//sunglasses.load("sunglasses.png");
	//ofEnableAlphaBlending();
	drawcam = false;
	b.init();
	ofBackground(ofColor(20, 20, 20));

	
	reset_game();
	faceCenter.set(CAM_WIDTH / 2.0, CAM_HEIGHT / 2.0);
	//ofRectMode(OF_RECTMODE_CENTER);


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

	float f_width = fabs(Z_FIELD_END - Z_FIELD_START);
	field = field.plane(f_width / 2., f_width, 11, 21);
	//field.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
	goalRect.set(ofGetWidth() / 2 - 200., ofGetHeight() - 100, 400, 200);


}


void ofApp::reset_game(void) {
	// draw the score and the game world
	ofApp::drop_faces = 0;
	score = 0;
	paddleSize = 300;
	state.set(S_IDLE, 0);
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
		faceRect.x = CAM_WIDTH - faceRect.x - faceRect.width;

		ofPoint fc = faceRect.getCenter();
		// scale center so we can reach the entire screen
		float border = SCREEN_WIDTH*(SCALE_FACE_POS - 1.);
		fc.x = ofMap(fc.x, 0., CAM_WIDTH, -border, SCREEN_WIDTH + border);
		border = SCREEN_HEIGHT*(SCALE_FACE_POS - 1.);
		fc.y = ofMap(fc.y, 0., CAM_HEIGHT,  -border, SCREEN_HEIGHT + border );
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
			state.set(S_FORWARD,0);
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
				state.set(S_BACKWARD,0);
				if (loc_flag) {
					// get distance of ball impact from paddle center
					ofPoint bc = ofPoint(b.pos.x, b.pos.y, b.pos.z);
					ofPoint pc = paddleRect.getCenter();
					// make it at the same z plane as ball
					ofVec3f spin = pc - bc;
					// we don't care about Z distance
					spin.z = 0;
					// compute return vector to make ball hit center of goal
					ofPoint gc = goalRect.getCenter();
					gc.z = Z_FIELD_END;
					// move goal center by scaled spin offset. Increasing spin means harder
					gc = gc + (4 * spin);
					ofVec3f straight = gc - bc;
					b.bounce(straight / 100.0);
					break;

				}
				else {
					cv::Vec2f v = finder.getVelocity(0);
					ofVec2f spin = toOf(v);

				}

			}
			else {
				state.set(S_IDLE,0);
			}

		}
		break;
	case S_BACKWARD:
		cout << "ball:" << b.getCenter() << " Z " << b.pos.z;
		cout << " goal " << goalRect.getCenter() << " Z " <<  Z_FIELD_END << "\n";

		if (b.pos.z < Z_FIELD_END) {
			b.reset();
			celebrate = Z_FAR / 2;
			score++;
			cout << "WIN!" << "score " << score << "\n";
			state.set(S_CELEBRATE,0);
		}
		break;
	case S_CELEBRATE:
		if (celebrate > 300) {
			b.reset();
			state.set(S_IDLE,0);
		}
		break;

	}

}

void ofApp::draw_world(void) {
	// draw the score and the game world
	//draw score


  
	for (int i = 0; i<arguments.size(); ++i) {
		ofDrawBitmapString(arguments.at(i), 20.0f, 20.0f*i);
	}

	ofPushMatrix();
	ofTranslate(0, 0, 0);
	ofSetColor(0, 255, 0);//stroke color 
	sprintf(score_str, "SCORE %02d", score);
	font.drawString(score_str, ofGetWidth() - 220, 50);
	//font.drawStringAsShapes("test", ofGetWidth() / 2 + 20, ofGetHeight() / 2);
	ofPopMatrix();

	// draw goal
	ofPushMatrix();
	ofSetColor(0, 128, 0, 128);
	ofFill();
	ofTranslate(0., 0., Z_FIELD_END);
	ofDrawRectRounded(goalRect, 20);
	ofPopMatrix();

	// draw field
	ofPushMatrix();
	ofTranslate(ofGetWidth() / 2., ofGetHeight(), Z_FIELD_START);
	//ofTranslate(ofGetWidth() / 2., ofGetHeight() / 2., 30);
	ofRotateX(90.);
	ofNoFill();
	ofSetColor(0, 255, 0);
	field.drawWireframe();
	ofPopMatrix();

}

void ofApp::draw() {
	float spinX = sin(ofGetElapsedTimef()*.35f);
	float spinY = cos(ofGetElapsedTimef()*.075f);

	ofPolyline debug;
	ofPoint gc = ofPoint(goalRect.getCenter());
	gc.z = Z_FIELD_END;
	ofPoint bc = ofPoint(b.pos.x, b.pos.y, b.pos.z);


	
	debug.addVertex(bc);
	debug.addVertex(gc);
	ofSetColor(255, 0, 0);
	debug.draw();

	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 50, 100);


	if (drawcam) {
		ofSetColor(255, 255, 255);//stroke color  
		cam.draw(CAM_WIDTH, 0, -CAM_WIDTH, CAM_HEIGHT);
	}

	draw_world();
	// future work, pick biggest face rectangle
	//for (int i = 0; i < finder.size(); i++) {

	// draw paddle
	ofNoFill();
	// Draw paddle
	for (float z = Z_NEAR; z > Z_NEAR - 151; z -= 50) {
		ofPushMatrix();
		ofSetColor(0, 255, 255); //stroke color 
		ofTranslate(0., 0., z);
		ofDrawRectRounded(paddleRect, 30);
		ofPopMatrix();
	}


	//ofPushMatrix();
	//ofTranslate(0, 0, Z_CLOSE);
	//ofDrawRectRounded(paddleRect, 50);
	//ofPopMatrix();

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

void ofApp::update_celebrate(int win) {

	celebrate += 10;

	ofPushMatrix();
	//ofTranslate(ofGetWidth() / 2., ofGetHeight() / 2., Z_FAR);


	ofTranslate(ofGetWidth() / 2., ofGetHeight() / 2., celebrate);
	if (win) {
		ofRotateX(float(celebrate / 2.));
		ofRectangle rect = font.getStringBoundingBox("GOAL!!!", 0, 0);
		font.drawStringAsShapes("GOAL!!!", -rect.width / 2., 0);

	}
	else {
		ofRectangle rect = font.getStringBoundingBox("MISS", 0, 0);
		font.drawStringAsShapes("MISS", -rect.width / 2., -Z_FAR - celebrate);
	}
	ofPopMatrix();

}




//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
	ofPoint gc = goalRect.getCenter();
	gc.z = Z_FIELD_END;
	ofPoint bc = ofPoint(b.pos.x, b.pos.y, b.pos.z);

	ofVec3f straight = gc - bc;


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

	case 'b':
		// bounce
		cout << "bounce request \n";
		cout << "ball:" << b.getCenter() << " Z " << b.pos.z;
		cout << " goal " << goalRect.getCenter() << " Z " << Z_FIELD_END << "\n";
		cout << " direction: " << straight << "\n";

		state.set(S_BACKWARD,0);
		b.bounce(straight / 100.0);
		break;

	case 'x':
		ofExit();
		break;

	}


}


// State machine handling ------------------------------------------------------------
void StateMach::set(int next_state, float timeout) {
	state = next_state;
	timeout_time = timeout;
	// default no timeout
	if (timeout >= 0) {
		ofResetElapsedTimeCounter();
		reset_timer();
	}
	print();
}

void StateMach::reset_timer(void) {
	start_time = ofGetElapsedTimef();
}

float StateMach::time_elapsed(void) {
	return(ofGetElapsedTimef() - start_time);
}

// return the fraction of elapsed time that has passed 
float StateMach::frac_time_elapsed(void) {
	if (time_elapsed() > timeout_time) {
		return 1.0;
	}
	return(time_elapsed() / timeout_time);
}


bool StateMach::timeout(void) {
	if (timeout_time < 0)
		return(true);
	if (time_elapsed() > timeout_time)
		return(true);
	return(false);
}

void StateMach::print(void) {
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
void ofApp::clean_exit(int signal) {
	// clean exit, signal handler
	cout << "FACEBALL Exit signal caught, bye!\n";
	ofExit();
}
