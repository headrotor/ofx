#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	
	int i;
	//ofSetFullscreen(true);
	for (i = 0; i < NUM; i++) {
		b[i].init(i);
	}	
}

//--------------------------------------------------------------
void ofApp::update() {
	for (int i = 0; i < NUM; i++) {
		b[i].update(mouseX, mouseY);
	}
}
//--------------------------------------------------------------
void ofApp::draw(){
	int i;
	for (i = 0; i < NUM; i++) {

		//isp.rotate(spinX, 1.0, 0.0, 0.0);
		//isp.rotate(spinY, 0, 1.0, 0.0);

		// get all the faces from the icoSphere, handy when you want to copy
		// individual vertices or tweak them a little ;)
		//vector<ofMeshFace> triangles = isp.getMesh().getUniqueFaces();

		// now draw
		b[i].draw();
	}
}

void Ball::init(int the_index) {
	index = the_index;
	isp.setRadius(50);
	z = (200 * index) - 2000;
	pos.set(ofGetWidth()/2, ofGetHeight()/2, z);
	bmesh = isp.getMeshPtr();
	int numVerts = (*bmesh).getNumVertices();
	ofColor green(0, 255, 0);
	for (int i = 0; i<numVerts; ++i) {
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

void Ball::update(float x, float y) {
	z -= 1.0;
	if (z < -2000) {
		z += 2000;
		pos.set(x, y, z);
	}
	//pos.set(ofGetWidth()*.1*index, ofGetHeight()*.07*index, z);
	pos.set(pos.x, pos.y, z);
	isp.setPosition(pos);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	cout << "bye";
	ofExit();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}