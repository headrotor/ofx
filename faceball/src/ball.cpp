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
	vel.set(ofRandomf()*1., ofRandomf()*0.5, forwardSpeed);
	pos.set(ofGetWidth()/2 + ofRandomf()*100, ofGetHeight()/2 + ofRandomf()*100., Z_FAR);
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

void Ball::set_velxy(float xv, float yv) {
	vel.x = xv;
	vel.y = yv;
}
void Ball::set_velz(float zv) {
	vel.z = zv;
}
void Ball::bounce(ofVec3f spin) {
	cout << "bounce spin: " << spin;
	//spin.z = -spin.z;
	vel = spin;
	//vel.x = spin.x;
	//vel.y = spin.y;
	// could do this better with spin as unit direction vector
}