#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void Goop::setup(ofImage img) {
	// allocate image storage

	w = img.getWidth();
	h = img.getHeight();

	pntr = 0;

	U.allocate(w, h, OF_IMAGE_GRAYSCALE);
	//U.clone(img);
	V.allocate(w, h, OF_IMAGE_GRAYSCALE);

	for (int i = 0; i < DEPTH; i++) {
		fp[i].allocate(w, h, OF_IMAGE_GRAYSCALE);

		for (int y = 0; y < h ; y++) {
			for (int x = 0; x < w; x++) {
				int j = y * w + x;
				fp[i][j] = 0.0;
			}
		}
	}
}

void Goop::updateWave(ofImage force) {

	unsigned char * Fpix = force.getPixels();
	unsigned char * Upix = U.getPixels();

	// kind of like tripple-buffering: we need times t+1, t, and t-1
	// so rotate through the stack of three floating point images...
	pntr = (pntr + 1) % DEPTH;

	float * vnew = fp[(pntr + 2) % DEPTH].getData();
	float * vold = fp[(pntr + 1) % DEPTH].getData();
	float * vvold = fp[(pntr + 0) % DEPTH].getData();

	for (int y = 1; y < h - 1; y++) {
		for (int x = 1; x < w - 1; x++) {
			int i = y * w + x;
			//vfp[i] = 1- (float)Ipix[i] / 255.0;
			// for this pixel, first compute Laplacian
			float v = 0.3*(vold[x - 1 + y*w] + vold[x + 1 + y*w] + vold[x + (y - 1)*w] + vold[x + (y + 1)*w] - 4 * vold[i]);
			// then add 2*v(t)
			v += 2 * vold[i];
			if (Fpix[i] > 0) {
				// forcing function
				vnew[i] = (float)Fpix[i] / 255.0;
			}
			else {
				//finally subtract v(t-1) with damping function
				vnew[i] = 0.99*(v - vvold[i]);
			}
		}
	}

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int i = y * w + x;
			float scale = 1.5;
			float temp = ofClamp(vnew[i], -scale, scale);
			//Upix[i] = 128 + (unsigned char)(127 * vnew[i]);
			Upix[i] = int(ofMap(temp, -scale, scale, 0, 255));
		}
	}
	U.update();
}

//--------------------------------------------------------------
void ofApp::setup(){
    
	// camera and face detect:
	ofSetFrameRate(120);
	finder.setup("haarcascade_frontalface_default.xml");
	finder.setPreset(ObjectFinder::Fast);
	finder.getTracker().setSmoothingRate(.3);
	cam.listDevices();
	cam.setDeviceID(0);
	cam.setup(IMG_W, IMG_H);


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
	goop.setup(img);

	
	plane.set(IMG_H, IMG_W, IMG_W / 10, IMG_H / 10);
	//plane.mapTexCoordsFromTexture(img.getTextureReference());
	plane.mapTexCoordsFromTexture(goop.U.getTextureReference());
	ofSetFrameRate(30);
}




//--------------------------------------------------------------
void ofApp::update() {


	cam.update();
	if (cam.isFrameNew()) {
		finder.update(cam);
	}

	if (finder.size() > 0) {

		int i = 0;
		faceRect = finder.getObjectSmoothed(i);
		// flip X coordinate
		faceRect.x = cam.getWidth() - faceRect.x - faceRect.width;

		fc = faceRect.getCenter();
	}

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
			//float d = sqrt((x - mouseX/10)*(x-mouseX/10) + (y-mouseY/10)*(y-mouseY/10));
			float facex = fc.x / float(cam.getWidth());
			float facey = fc.y / float(cam.getHeight());
			int lx = int(facex * w);
			int ly = int(facey * h);

			float d = sqrt((x - lx)*(x - lx) + (y - ly)*(y -ly));

			if ((d < 10) && ofGetMousePressed()) {
				pixels[i] = 255 - 2.5*d*d;
			}
			else {
				pixels[i] = 0;
			}
		}
	}
	img.update();
	goop.updateWave(img);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // bind our texture. in our shader this will now be tex0 by default
    // so we can just go ahead and access it there.
	//img.getTextureReference().bind();
	goop.U.getTextureReference().bind();

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
	goop.U.draw(img.getWidth(), 0);
	if (false) {
		ofPushMatrix();
		ofTranslate(2 * img.getWidth(), 0);
		cam.draw(IMG_W, 0, -IMG_W, IMG_H);
		ofRect(faceRect);
		ofPopMatrix();
	}
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