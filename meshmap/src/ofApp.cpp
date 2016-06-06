#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
#ifdef TARGET_OPENGLES
	shader.load("shadersES2/shader");
#else
	if(ofIsGLProgrammableRenderer()){
		shader.load("shadersGL3/shader");
	}else{
		shader.load("shadersGL2/shader");
	}
#endif

	img.allocate(IMG_W / 10, IMG_H / 10, OF_IMAGE_GRAYSCALE);
	U.allocate(IMG_W / 10, IMG_H / 10, OF_IMAGE_GRAYSCALE);
	//U.clone(img);
	V.clone(img);


    plane.set(IMG_H, IMG_W, IMG_W / 10, IMG_H / 10);
    plane.mapTexCoordsFromTexture(img.getTextureReference());
}

void ofApp::updateWave() {
	unsigned char * Ipix = img.getPixels();
	unsigned char * Upix = U.getPixels();
	int w = img.getWidth();
	int h = img.getHeight();
	for (int y = 0; y<h; y++) {
		for (int x = 0; x<w; x++) {
			int i = y * w + x;
			Upix[i] = Ipix[i];
		}
	}
	U.update();
}




//--------------------------------------------------------------
void ofApp::update() {
	float noiseScale = ofMap(mouseX, 0, ofGetWidth(), 0, 0.1);
    float noiseVel = ofGetElapsedTimef();
    
    unsigned char * pixels = img.getPixels();
    int w = img.getWidth();
    int h = img.getHeight();
    /*
	for(int y=0; y<h; y++) {
        for(int x=0; x<w; x++) {
            int i = y * w + x;
            float noiseVelue = ofNoise(x * noiseScale, y * noiseScale, noiseVel);
            pixels[i] = 255 * noiseVelue;
        }
    }*/
	for (int y = 0; y<h; y++) {
		for (int x = 0; x<w; x++) {
			int i = y * w + x;
			float d = sqrt((x - mouseX/10)*(x-mouseX/10) + (y-mouseY/10)*(y-mouseY/10));
			if ((d < 10) && ofGetMousePressed()) {
				pixels[i] = 255 - 2.5*d*d;
			}
			else {
				pixels[i] = 0;
					}
		}
	}
    img.update();
	updateWave();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // bind our texture. in our shader this will now be tex0 by default
    // so we can just go ahead and access it there.
    img.getTextureReference().bind();
    
    shader.begin();

    ofPushMatrix();
    
    // translate plane into center screen.
    float tx = ofGetWidth() / 2;
    float ty = ofGetHeight() / 2;
    ofTranslate(tx, ty);

    // the mouse/touch Y position changes the rotation of the plane.
    float percentY = mouseY / (float)ofGetHeight();
    float rotation = ofMap(percentY, 0, 1, -60, 60, true) + 60;
    ofRotate(rotation, 1, 0, 0);

    plane.drawWireframe();
    
    ofPopMatrix();
    
    shader.end();

	ofSetColor(ofColor::white);
	img.draw(0, 0);
	ofSetColor(ofColor::white);
	U.draw(img.getWidth(), 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}