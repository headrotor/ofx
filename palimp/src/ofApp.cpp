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
			id = 0;
			float max_w = -1.;
			// find biggest face
			ofRectangle r;
			for (int i = 0; i < finder.size(); i++) {
				r = finder.getObjectSmoothed(i);
				if (r.width > max_w) {
					id = i;
					facerect = r;
					max_w = r.width;
				}
			}
			cv::Vec2f v = finder.getVelocity(id);
			vel = toOf(v);
			facerect = finder.getObjectSmoothed(id);
		}
	}

	// state machine handling
	switch (state.state) {
	case S_IDLE:
		//update_idle();
		if (finder.size()) {
			// found face, so go to capture mode
			// check for face size here
			if (state.timeout() && (facerect.width > ofGetWidth()/3.0)) {
				avg_xvel = 0;
				avg_yvel = 0;
				// disabled to test idle
				//state.set(S_HELLO, 5.);
				// grab image when first detected
				saveimg.setFromPixels(cam.getPixelsRef());
				saverect = finder.getObjectSmoothed(0);
			}
		}
		break;

	case S_HELLO:
		if (finder.size() == 0) {
			//cout << "timer: " << state.time_elapsed() << "\n";
			if (state.timeout()) {
				state.set(S_NO_IMG, 2.);
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
		draw_idle();
		break;

	case S_HELLO:
		msg.drawString("Hello!", 50, 100);
		msg.drawString("May we take your picture?", 50, ofGetHeight() - 150);
		msg.drawString("(nod yes or no)", 50, ofGetHeight() - 75);
		break;

	case S_YES_IMG:
		msg.drawString("Thank you!", 100, 300);
		break;

	case S_NO_IMG:
		msg.drawString("OK, thanks anyway!", 50, ofGetHeight() - 100);
		break;
	}

}

void ofApp::init_idle() {
	//some path, may be absolute or relative to bin/data
	cout << "init idle\n";
	string path = "";
	ofDirectory imgs(path);
	//only show png files
	imgs.allowExt("png");
	//populate the directory object
	imgs.listDir();

	if (imgs.size() > 0) {
		cout << "loading file " << imgs.getPath(imgs.size()-1) << "\n";
		idle_image.load(imgs.getFile(imgs.size()-1));
	}	
	//go through and print out all the paths
	for (int i = 0; i < imgs.size(); i++) {
		ofLogNotice(imgs.getPath(i));
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
	sprintf(name, "%sx%03dy%03dw%03dh%03d.png", timestamp, int(r.x), int(r.y), int(r.width), int(r.height));
	cout << "saving image: " << name;
	saveimg.saveImage(name);
	id++;

}
//--------------------------------------------------------------
void ofApp::draw_idle() {
	// list of images in ofDirectory imgs, load and display
	ofSetColor(255, 255, 255, int(127 * (sin(ofGetElapsedTimef() + 1.))));
	idle_image.draw(0, 0);

}


void ofApp::keyPressed(int key) {
	if (key == 's') {
		// test back-to-idle handling
		saveimg.setFromPixels(cam.getPixelsRef());
		saverect = finder.getObjectSmoothed(0);
		store_image();
		init_idle();
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