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
	cam.setup(CAMW, CAMH);
	//sunglasses.load("sunglasses.png");
	ofEnableAlphaBlending();
	id = 0;

	cropr = ofRectangle(CROPX, CROPY, CROPW, CROPH);
	grabimg.allocate(CAMW, CAMH, OF_IMAGE_COLOR);
	cropimg.allocate(CROPW, CROPH, OF_IMAGE_GRAYSCALE);
	colorimg.allocate(CAMW, CAMH);
	grayimg.allocate(CAMW, CAMH);
}

void ofApp::update() {
	ofVec2f vel;
	cam.update();
	if(cam.isFrameNew()) {
		grabimg.setFromPixels(cam.getPixels());
		cropimg.cropFrom(grabimg, CROPX, CROPY, CROPH, CROPW);
		colorimg.setFromPixels(grabimg.getPixels());

		// convert to grayscale
		grayimg = colorimg;
		// do face detection in grayscale
		finder.update(grayimg);
		//cropimg = grayimg(cropr);
		if (finder.size() > 0) {
			cv::Vec2f v = finder.getVelocity(0);
			vel = toOf(v);
			if (vel.length() > 1.0) {
				cout << "Yvel:" << setprecision(4) << vel.y;
				//cout << "Yvel:" <<  vel.y / vel.length();
				cout << "\n";
			}
		}

	}

	// look for motion in crop region


}

void ofApp::draw() {
	ofSetHexColor(0xFFFFFF);
	grayimg.draw(0, 0);
	cropimg.draw(0, 0);
	ofSetHexColor(0xCCCCCC);
	ofDrawRectRounded(cropr.x, cropr.y, cropr.width, cropr.height, 30.0);
	
	for(int i = 0; i < finder.size(); i++) {
		ofRectangle object = finder.getObjectSmoothed(i);
		//sunglasses.setAnchorPercent(.5, .5);
		saveimg.setFromPixels(cam.getPixelsRef());
		//float scaleAmount = .85 * object.width / sunglasses.getWidth();
		//float scaleAmount = 1.1 * object.width / sunglasses.getWidth();
		ofNoFill();
		ofSetHexColor(0xFFFF00);
		ofDrawRectRounded(object.x, object.y, object.width, object.height, 30.0);

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

}