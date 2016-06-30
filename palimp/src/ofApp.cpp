#include "ofApp.h"

// for time stamps 
#include <ctime>

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
	state.set(S_IDLE, 0);

	msg.loadFont("impact.ttf", 36, true, false, true, 0.1);
	facerect = ofRectangle(0, 0, 0, 0);
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
		vel.set(0, 0);
		if (finder.size() > 0) {
			cv::Vec2f v = finder.getVelocity(0);
			vel = toOf(v);
			facerect = finder.getObjectSmoothed(0);
		}
	}

	// state machine handling
	switch (state.state) {
	case S_IDLE:
		if (finder.size()) {
			// found face, so go to capture mode
			// check for face size here
			if (state.timeout()) {
				avg_xvel = 0;
				avg_yvel = 0;
				state.set(S_HELLO, 5.);
				// grab image when first detected
				saveimg.setFromPixels(cam.getPixelsRef());
				saverect = finder.getObjectSmoothed(0);
			}
		}
		break;

	case S_HELLO:
		if (finder.size() == 0) {
			cout << "timer: " << state.time_elapsed() << "\n";
			if (state.timeout()) {
				state.set(S_IDLE, 10.);
			}
		}
		else {
			float mix = 0.7;
			avg_xvel = mix*fabs(vel.x) + (1 - mix)*avg_xvel;
			avg_yvel = mix*fabs(vel.y) + (1 - mix)*avg_yvel;
			//cout << std::fixed << std::showpoint;
			//cout << "Y vel:" << setprecision(3) << avg_xvel;
			//cout << "    X vel:" << setprecision(3) << avg_yvel;
			//cout << "\n";
			if (avg_yvel > 10.0) {
				state.set(S_YES_IMG, 3.);
				store_image();
			}
			if (avg_xvel > 15.0) {
				state.set(S_NO_IMG, 3.);
			}
		}
		break;

	case S_YES_IMG:
		if (state.timeout()) {
			state.set(S_IDLE, 2);
		}
		break;

	case S_NO_IMG:
		if (state.timeout()) {
			state.set(S_IDLE, 2);
		}
		break;
	}
}


void ofApp::draw() {
	ofSetHexColor(0xFFFFFF);
	grayimg.draw(0, 0);
	ofSetHexColor(0xCCCCCC);


	//sunglasses.setAnchorPercent(.5, .5);
	//float scaleAmount = .85 * object.width / sunglasses.getWidth();
	//float scaleAmount = 1.1 * object.width / sunglasses.getWidth();
	ofNoFill();
	ofSetHexColor(0xFFFF00);
	ofDrawRectRounded(facerect.x, facerect.y, facerect.width, facerect.height, 30.0);

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

	switch (state.state) {
	case S_IDLE:
		break;

	case S_HELLO:
		msg.drawString("Hello!", 50, 100);
		msg.drawString("May we take your picture?", 50, ofGetHeight() - 100);
		break;

	case S_YES_IMG:
		msg.drawString("Thank you!", 100, 300);
		break;

	case S_NO_IMG:
		msg.drawString("OK, thanks anyway!", 50, ofGetHeight() - 100);
		break;
	}

}

void ofApp::store_image() {

	time_t rawtime;
	time(&rawtime);
	struct tm *timeinfo;
	timeinfo = localtime(&rawtime);
	char timestamp[80];
	strftime(timestamp, 80, "%m-%d-%H-%M-%S", timeinfo);
	cout << timestamp << std::endl;
	ofRectangle r = facerect;
	char name[256];
	sprintf(name, "%sx%03dy%03dw%d03h%03d.png", timestamp, int(r.x), int(r.y), int(r.width), int(r.height));
	cout << "saving image: " << name;
	saveimg.saveImage(name);
	id++;

}
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 's') {
		store_image();
	}

	if (key == 'p')
	{
		time_t rawtime;
		time(&rawtime);
		struct tm *timeinfo;
		timeinfo = localtime(&rawtime);
		//char buffer[80];
		//strftime(buffer, 80, "Now it's %I:%M%p.", timeinfo);
		char timestamp[80];
		strftime(timestamp, 80, "%m-%d-%H-%M-%S", timeinfo);
		//cout << "Current time is ::  " << ctime(&rawtime) << std::endl;
		cout << timestamp << std::endl;
	}

}


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

bool StateMach::timeout(void) {
	if (timeout_time < 0)
		return(false);
	if (time_elapsed() > timeout_time)
		return(true);
	return(false);
}

void StateMach::print(void) {

	switch (state) {
	case S_IDLE:
		cout << "State: IDLE\n";
		break;
	case S_HELLO:
		cout << "State: HELLO\n";
		break;
	case S_CAPTURE:
		cout << "State: CAPTURE\n";
		break;
	case S_NO_IMG:
		cout << "State: NO_IMG\n";
		break;
	case S_YES_IMG:
		cout << "State: YES_IMG\n";
		break;
	}
}