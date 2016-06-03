#include "ofApp.h"

void Ball::init(int the_index) {
	index = the_index;
	isp.setRadius(50);
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
	reset();
}


void Ball::reset(void) {
	vel.set(5.0, 5.0, 5);
	pos.set(0, 0, Z_FAR);
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
	vel.z = -vel.z;

}