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
	cam.setup(CAMW, CAMH);
	//sunglasses.load("sunglasses.png");
	ofEnableAlphaBlending();
	id = 0;

	grabimg.allocate(CAMW, CAMH, OF_IMAGE_COLOR);
	colorimg.allocate(CAMW, CAMH);
	grayimg.allocate(CAMW, CAMH);

	// state machine handling
	state.set(S_IDLE);

}

void ofApp::update() {
	ofVec2f vel;
	cam.update();
	if (cam.isFrameNew()) {
		grabimg.setFromPixels(cam.getPixels());
		colorimg.setFromPixels(grabimg.getPixels());

		// convert to grayscale
		grayimg = colorimg;
		// do face detection in grayscale
		finder.update(grayimg);
		//cropimg = grayimg(cropr);
		if (finder.size() > 0) {
			cv::Vec2f v = finder.getVelocity(0);
			vel = toOf(v);
			if (vel.length() > 1.0) {
				cout << "Yvel:" << setprecision(4) << vel.y;
				//cout << "Yvel:" <<  vel.y / vel.length();
				cout << "\n";
			}
		}

	}

	// state machine handling

	switch (state.state) {
	case S_IDLE:
		if (finder.size()) {
			// found face, so go to capture mode
			state.set(S_CAPTURE);
		}
		break;

	case S_CAPTURE:
		if (finder.size() == 0) {
			cout << "timer: " << state.time_elapsed();
			if (state.timeout()) {
				state.set(S_IDLE);
			}
		}
		else {
			state.reset_timer();
		}

	}
	// look for motion in crop region


}

void ofApp::draw() {
	ofSetHexColor(0xFFFFFF);
	grayimg.draw(0, 0);
	ofSetHexColor(0xCCCCCC);

	for (int i = 0; i < finder.size(); i++) {
		ofRectangle object = finder.getObjectSmoothed(i);
		//sunglasses.setAnchorPercent(.5, .5);
		saveimg.setFromPixels(cam.getPixelsRef());
		//float scaleAmount = .85 * object.width / sunglasses.getWidth();
		//float scaleAmount = 1.1 * object.width / sunglasses.getWidth();
		ofNoFill();
		ofSetHexColor(0xFFFF00);
		ofDrawRectRounded(object.x, object.y, object.width, object.height, 30.0);

		//ofPushMatrix();
		//ofTranslate(object.x + object.width / 2., object.y + object.height * 1.1);
		//ofScale(scaleAmount, scaleAmount);
		//sunglasses.draw(0, 0);
		//ofPopMatrix();

		//ofPushMatrix();
		//ofTranslate(object.getPosition());
		//ofDrawBitmapStringHighlight(ofToString(finder.getLabel(i)), 0, 0);
		//ofDrawLine(ofVec2f(), toOf(finder.getVelocity(i)) * 10);
		//ofPopMatrix();
	}
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'p')
	{
		char name[256];
		sprintf(name, "img%03d.png", id);
		saveimg.saveImage(name);
		id++;
		cout << "P pressed\n";
	}

}


void StateMach::set(int next_state) {
	reset_timer();
	state = next_state;
	// default no timeout
	timeout_time = -1.;

	// do state-dependent things
	switch (state) {
	case S_IDLE:
		ofResetElapsedTimeCounter();
		reset_timer();
		break;
	case S_CAPTURE:
		timeout_time = 5.;
		break;
	}
	print();
}

void StateMach::reset_timer(void) {
	start_time = ofGetElapsedTimef();
}

float StateMach::time_elapsed(void) {
	return(ofGetElapsedTimef() - start_time);
}

bool StateMach::timeout(void) {
	if (timeout_time < 0)
		return(false);
	if (time_elapsed() >  timeout_time)
		return(true);
	return(false);
}

void StateMach::print(void) {

	switch (state) {
	case S_IDLE:
		cout << "State: IDLE\n";
		break;
	case S_CAPTURE:
		cout << "State: CAPTURE\n";
		break;
	case S_SAVE:
		cout << "State: SAVE\n";
		break;
	case S_INTERIM:
		cout << "State: CELEBRATE\n";
		break;
	}
}