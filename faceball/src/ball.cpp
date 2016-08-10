#include "ofApp.h"

void Ball::init(double speed) {
	isp.setRadius(50);
	isp.setResolution(1);
	ofColor green(0, 255, 0);
	set_color(green);
	forwardSpeed = speed;
	reset();
}

void Ball::set_color(ofColor c) {
	bmesh = isp.getMeshPtr();
	int numVerts = (*bmesh).getNumVertices();
	for (int i = 0; i < numVerts; ++i) {
		ofVec3f vert =
			(*bmesh).getVertex(i);

		//float time = ofGetElapsedTimef();
		//float timeScale = 5.0;
		//float displacementScale = 10;

		(*bmesh).addColor(c);
		(*bmesh).setVertex(i, vert);
	}
}

void Ball::reset() {


	pos.set(ofGetWidth()/2 + ofRandomf()*1000, ofGetHeight()/2 + ofRandomf()*1000., Z_FAR);
	ofVec3f dest = ofPoint(ofGetWidth()*ofRandomuf(),  ofGetHeight()*ofRandomuf(), Z_NEAR);
	// figure out vector from ball origin to random place in viewer plane
	ofVec3f traj = dest - pos;
	vel = traj *forwardSpeed;
	// figure out vector from ball origin to random place in viewer plane

}

void Ball::draw() {

	isp.drawWireframe();
}

ofVec3f Ball::getCenter() {
	return pos;
}

void Ball::update(int state) {
	pos = pos + vel*ofGetLastFrameTime();
	//pos.set(ofGetWidth()*.1*index, ofGetHeight()*.07*index, z);
	//pos.set(pos.x, pos.y + 0.007*z, z);
	isp.setPosition(pos);
	float spinX = sin(ofGetElapsedTimef()*.03f);
	float spinY = cos(ofGetElapsedTimef()*.005f);
	isp.rotate(spinX, 1.0, 0.0, 0.0);
	isp.rotate(spinY, 0, 1.0, 0.0);

	//cout << "ball " << pos.z << "\n";
}

void Ball::bounce(ofVec3f spin) {
	cout << "bounce spin: " << spin;
	//spin.z = -spin.z;
	vel = spin;
	//vel.x = spin.x;
	//vel.y = spin.y;
	// could do this better with spin as unit direction vector
}