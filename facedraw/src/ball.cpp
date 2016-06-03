#include "ofApp.h"

void Ball::init(void) {
	isp.setRadius(50);

	ofColor green(0, 255, 0);
	set_color(green);
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

void Ball::reset(void) {
	//vel.set(5.0, 5.0, 5);
	//pos.set(0, 0, Z_FAR);
	vel.set(ofRandomf()*1.0, ofRandomf()*0.5, 5);
	pos.set(CAM_WIDTH/2., CAM_HEIGHT/2, Z_FAR);
}
void Ball::draw() {
	isp.drawWireframe();
}

void Ball::update(int state) {
	pos = pos + vel;
	//pos.set(ofGetWidth()*.1*index, ofGetHeight()*.07*index, z);
	//pos.set(pos.x, pos.y + 0.007*z, z);
	isp.setPosition(pos);

	//cout << "ball " << pos.z << "\n";
}

void Ball::set_velxy(float xv, float yv) {
	vel.x = xv;
	vel.y = yv;
}
void Ball::set_velz(float zv) {
	vel.z = zv;
}
void Ball::bounce(void) {
	vel.z = -10.0;

}