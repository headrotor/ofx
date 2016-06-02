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
	//cam.setup(CAM_WIDTH, CAM_HEIGHT);
	cam.setup(640,480);
	sunglasses.load("sunglasses.png");
	ofEnableAlphaBlending();
	id = 0;
	drawcam = false;
	for (int i = 0; i < NUM; i++) {
		b[i].init(i);
	}
	ofBackground(ofColor(20, 20, 20));

	state.set_state(S_IDLE);
	faceCenter.set(CAM_WIDTH/2.0,CAM_HEIGHT/2.0);
}

void ofApp::update() {

	cam.update();
	if (cam.isFrameNew()) {
		finder.update(cam);
	}


	if (finder.size() > 0) {
		
		int i = 0;
		
		ofRectangle object = finder.getObjectSmoothed(i);
		faceCenter = object.getCenter();
		float xc = ofGetWidth() - object.x - object.width / 2;
		float yc = object.y + object.height / 2;
		for (int i = 0; i < NUM; i++) {

			b[i].update(state.state);
		}

	}
	else {

		for (int i = 0; i < NUM; i++) {
			b[i].update(state.state);
		}
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
		break;
	case S_BACKWARD:
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
		ofDrawRectRounded(facerect, 50);
	}

	for (int i = 0; i < NUM; i++) {
		b[i].draw();
	}


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

void Ball::init(int the_index) {
	index = the_index;
	isp.setRadius(50);
	z = -2000;
	vel.set(5.0, 5.0, 10.);
	pos.set(0, 0, z);
	bmesh = isp.getMeshPtr();
	int numVerts = (*bmesh).getNumVertices();
	ofColor green(0, 255, 0);
	for (int i = 0; i < numVerts; ++i) {
		ofVec3f vert =
			(*bmesh).getVertex(i);

		//float time = ofGetElapsedTimef();
		//float timeScale = 5.0;
		//float displacementScale = 10;


		(*bmesh).addColor(green);
		(*bmesh).setVertex(i, vert);
	}

}

void Ball::draw() {
	isp.drawWireframe();
}

void Ball::update(int state) {
	pos = pos + vel;
	//z -= 10.0;
	//if (face && (z < -2000)) {
	//	z += 2000;
	//	pos.set(x, y, z);
	//}
	//pos.set(ofGetWidth()*.1*index, ofGetHeight()*.07*index, z);
	//pos.set(pos.x, pos.y + 0.007*z, z);
	isp.setPosition(pos);
}

void Ball::set_velxy(float xv, float yv) {
	vel.x = xv;
	vel.y = yv;
}
void Ball::set_velz(float zv) {
	vel.z = zv;
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
		cout << "State: RETURN\n";
		break;

	}
}