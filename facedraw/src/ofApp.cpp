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
	cam.setup(640, 480);
	sunglasses.load("sunglasses.png");
	ofEnableAlphaBlending();
	id = 0;
	drawcam = false;
	for (int i = 0; i < NUM; i++) {
		b[i].init(i);
	}
	ofBackground(ofColor(20, 20, 20));
}

void ofApp::update() {

	cam.update();
	if (cam.isFrameNew()) {
		finder.update(cam);
	}
	if (finder.size() > 0) {
		int i = 0;
		ofRectangle object = finder.getObjectSmoothed(i);

		float xc = ofGetWidth() - object.x - object.width / 2;
		float yc = object.y + object.height / 2;
		for (int i = 0; i < NUM; i++) {

			b[i].update(xc, yc, true);
		}

	}
	else {

		for (int i = 0; i < NUM; i++) {
			b[i].update(0, 0, false);
		}
	}

}

void ofApp::draw() {
	if (drawcam) {
		ofSetColor(255, 255, 255);//stroke color  
		cam.draw(640, 0, -640, 480);
	}

	for (int i = 0; i < finder.size(); i++) {
		ofRectangle facerect = finder.getObjectSmoothed(i);
		facerect.x = ofGetWidth() - facerect.x - facerect.width;
		//ofFill();
		//ofSetColor(255, 255, 255); //fill color  
		//ofRect(x, y, width, height);
		ofNoFill();
		ofSetColor(0, 255, 255);//stroke color  
								//ofRect(x, y, width, height);
		ofDrawRectangle(facerect);
	}

	for (int i = 0; i < NUM; i++) {
		b[i].draw();
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
	if (key == 'f') {
			drawcam = !drawcam;
	}

}

void Ball::init(int the_index) {
	index = the_index;
	isp.setRadius(50);
	z = (200 * index) - 2000;
	pos.set(ofGetWidth() / 2, ofGetHeight() / 2, z);
	bmesh = isp.getMeshPtr();
	int numVerts = (*bmesh).getNumVertices();
	ofColor green(0, 255, 0);
	for (int i = 0; i < numVerts; ++i) {
		ofVec3f vert =
			(*bmesh).getVertex(i);

		//float time = ofGetElapsedTimef();
		//float timeScale = 5.0;
		//float displacementScale = 10;

		// A typical design pattern for using Perlin noise uses a couple parameters:
		// ofSignedNoise(time*timeScale+timeOffset)*displacementScale
		//     ofSignedNoise(time) gives us noise values that change smoothly over
		//         time
		//     ofSignedNoise(time*timeScale) allows us to control the smoothness of
		//         our noise (smaller timeScale, smoother values)
		//     ofSignedNoise(time+timeOffset) allows us to use the same Perlin noise
		//         function to control multiple things and have them look as if they
		//         are moving independently
		//     ofSignedNoise(time)*displacementScale allows us to change the bounds
		//         of the noise from [-1, 1] to whatever we want
		// Combine all of those parameters together, and you've got some nice
		// control over your noise

		//vert.x += (ofSignedNoise(time*timeScale )) * displacementScale;
		//vert.y += (ofSignedNoise(time*timeScale )) * displacementScale;
		//vert.z += (ofSignedNoise(time*timeScale )) * displacementScale;
		(*bmesh).addColor(green);
		(*bmesh).setVertex(i, vert);
	}

}

void Ball::draw() {
	isp.drawWireframe();
}

void Ball::update(float x, float y, bool face) {
	z -= 10.0;
	if (face && (z < -2000)) {
		z += 2000;
		pos.set(x, y, z);
	}
	//pos.set(ofGetWidth()*.1*index, ofGetHeight()*.07*index, z);
	pos.set(pos.x, pos.y + 0.007*z, z);
	isp.setPosition(pos);
}

