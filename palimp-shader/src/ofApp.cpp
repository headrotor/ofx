#include "ofApp.h"

// for time stamps 
#include <ctime>

using namespace ofxCv;
using namespace cv;


void ofApp::setup() {

	// init shaders 
	//shader.load("shadersES2/mix-shader");
#ifdef TARGET_OPENGLES
	shader.load("shadersES2/mix-shader");
#else
	if (ofIsGLProgrammableRenderer()) {
		shader.load("shadersGL3/mix-shader");
	}
	else {
		shader.load("shadersGL2/mix-shader");
	}
#endif

	// signal handler for clean exit

	image.loadImage("img.jpg");


	// load config file
	config.loadFile("config.xml");
	int frameRate;
	if (config.tagExists("config:frameRate", 0)) {
		frameRate = config.getValue("config:frameRate", 30);
		cout << "\nframe rate from config file" << frameRate << "\n";
	}
	else {
		cout << "\nerror reading frameRate from xml config file\n";
		cout << "\nframe rate from config file" << frameRate << "\n";
	}

	double smoothingRate;
	if (config.tagExists("config:smoothingRate", 0)) {
		smoothingRate = config.getValue("config:smoothingRate", 0.2);
		cout << "smoothing rate from config file: " << smoothingRate << "\n";
	}
	else {
		cout << "error reading smoothingRate from xml config file\n";
		smoothingRate = 0.2;
	}

	int cannyPruning;
	if (config.tagExists("config:cannyPruning", 0)) {
		cannyPruning = config.getValue("config:cannyPruning", 0);
	}
	else {
		cout << "error reading cannyPruning from xml config file\n";
		cannyPruning = 1;
	}

	if (config.tagExists("config:face_dropped_threshold")) {
		face_dropped_threshold = config.getValue("config:face_dropped_threshold", FACE_DROPPED_THRESHOLD);
	}
	else {
		cout << "error reading face_dropped_threshold from xml config file\n";
		face_dropped_threshold = FACE_DROPPED_THRESHOLD;
	}

	if (config.tagExists("config:nod_threshold")) {
		nod_threshold = config.getValue("config:nod_threshold", NOD_THRESHOLD);
	}
	else {
		cout << "error reading nod_threshold from xml config file\n";
		nod_threshold = NOD_THRESHOLD;
	}

	if (config.tagExists("config:num_images")) {
		num_images = config.getValue("config:num_images", NUM_IMAGES);
		cout << "num_images from config file: " << num_images << "\n";
	}
	else {
		cout << "error reading num_images from xml config file\n";
		num_images = NUM_IMAGES;
	}


#ifdef RPI
	if (signal(SIGUSR1, clean_exit) == SIG_ERR) {
		cout << "can't catch signal\n";
	}
#endif
	ofSetVerticalSync(true);
	ofSetFrameRate(frameRate);

	// smaller = smoother
	//finder.getTracker().setSmoothingRate(smoothingRate);
	// ignore low-contrast regions
	finder.setCannyPruning((cannyPruning > 0));

	finder.setup("haarcascade_frontalface_default.xml");
	finder.setPreset(ObjectFinder::Fast);
	finder.setFindBiggestObject(true);
	finder.getTracker().setSmoothingRate(smoothingRate);

	//cam.listDevices();
	cam.setDeviceID(0);
	cam.setup(CAM_WIDTH, CAM_HEIGHT);
	ofEnableAlphaBlending();
	id = 0;

	grabimg.allocate(CAM_WIDTH, CAM_HEIGHT, OF_IMAGE_COLOR);
	colorimg.allocate(CAM_WIDTH, CAM_HEIGHT);
	grayimg.allocate(CAM_WIDTH, CAM_HEIGHT);
	// something to display if facefinder doesn't
	grayimg.set(127.0);

	// state machine handling
	state.set(S_IDLE, 0);

	// allocate array of images 
	for (int i = 0; i < num_images; i++) {
		gray_images.push_back(ofImage());
		gray_rects.push_back(ofRectangle());
	}

	msg.loadFont("impact.ttf", 36, true, false, true, 0.1);
	facerect = ofRectangle(0, 0, 0, 0);

	// load stored images 
	init_idle();

	// setup shader FBOs
	//fbo.allocate(SCREEN_WIDTH, SCREEN_HEIGHT);
	fbo.allocate(image.getWidth(),image.getHeight());
	maskFbo.allocate(SCREEN_WIDTH, SCREEN_HEIGHT);

}

void ofApp::update() {
	ofVec2f vel;
	cam.update();
	if (cam.isFrameNew()) {
		grabimg.setFromPixels(cam.getPixels());
		colorimg.setFromPixels(grabimg.getPixels());

		// convert to grayscale
		grayimg = colorimg;
		// do face detection in grayscale
		finder.update(grayimg);
		//cropimg = grayimg(cropr);
		vel.set(0, 0);
		if (finder.size() > 0) {
			cv::Vec2f v = finder.getVelocity(0);
			vel = toOf(v);
			facerect = finder.getObjectSmoothed(0);
			// scale by size of camera and face rect to normalize motion
			vel.x *= 500. / facerect.x;
			vel.y *= 500. / facerect.y;

			//cout << "velx:" << setw(6) << setprecision(2) << vel.x;
			//cout << " vely: " << setw(6) << setprecision(2) << vel.y << "\n";

			float mix = 0.3;
			avg_xvel = mix*abs(vel.x) + 0.9*(1 - mix)*avg_xvel;
			avg_yvel = mix*abs(vel.y) + 0.9*(1 - mix)*avg_yvel;
			/*
			cout << " avg_x:" << fixed << setw(6) << setprecision(1) << avg_xvel;
			cout << " avg_y: " << fixed << setw(6) << setprecision(1) << avg_yvel << "\n";
			if (avg_xvel > 35)
				cout << "X NO----";
			else
				cout << "        ";
			if (avg_yvel > 25)
				cout << "Y+YES+++\n";
			else
				cout << "        \n";
			*/

		}
	}

	// state machine handling
	switch (state.state) {
	case S_IDLE:
		if (false) { //disable for testing
		//if (finder.size() > 0) {
				// found face, so go to capture mode
			// check for face size here
			if (state.timeout() && (facerect.width*xscale > ofGetWidth() / 5.0)) {

				state.set(S_HELLO, 2.);
				// grab image when first detected

			}
		}
		break;

	case S_HELLO:
		/*
			if (finder.size() == 0) {
				state.set(S_NO_IMG, 2.);
			}
			*/
			//update_idle();
		yes_flag = false;
		avg_xvel = 0;
		avg_yvel = 0;
		if (state.timeout()) {
			state.set(S_QUESTION, 5);
		}

	case S_QUESTION:
		if (state.timeout()) {
			// no obvious yes, so default to no
			state.set(S_NO_IMG, 2.);
		}
		if (finder.size() > 0) {
			if (avg_yvel > nod_threshold) {
				state.set(S_THREE, 1.);
			}
			//if (avg_xvel > 45.0) {
			//	state.set(S_NO_IMG, 2.);
			//	yes_flag = false;
			//}
		}
		break;

	case S_THREE:
		if (state.timeout()) {
			state.set(S_TWO, 1.);
		}

	case S_TWO:
		if (state.timeout()) {
			state.set(S_ONE, 1.);
		}

	case S_ONE:
		if (state.timeout()) {
			state.set(S_CAPTURE, 2);
		}

	case S_CAPTURE:
		//if (finder.size() == 0) {
		//	state.set(S_NO_IMG, 2.);
		//}
		if (state.timeout()) {
			state.set(S_YES_IMG, 2);
			saveimg.setFromPixels(cam.getPixels());
			saverect = finder.getObjectSmoothed(0);
			store_image();
			yes_flag = true;
		}

	case S_YES_IMG:
		if (state.timeout()) {
			state.set(S_IDLE, 10);
		}
		break;

	case S_NO_IMG:
		if (state.timeout()) {
			state.set(S_IDLE, 10);
		}
		break;
	}
}

void ofApp::draw() {


	fbo.begin();
	ofClear(0, 0, 127, 255);

	shader.begin();
	shader.setUniformTexture("tex0", image, 1);
	//shader.setUniformTexture("tex1", grayimg, 2);
	//shader.setUniformTexture("tex2", grayimg, 3);
	//shader.setUniformTexture("tex1", image, 2);
	//shader.setUniformTexture("tex2", movie.getTextureReference(), 3);
	//shader.setUniformTexture("imageMask", maskFbo.getTextureReference(), 4);

	// we are drawing this fbo so it is used just as a frame.
	maskFbo.draw(0, 0);

	shader.end();
	fbo.end();

	fbo.draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	fbo.draw(0, 0, 320, 240);
	image.draw(320, 240, 320, 240);

	return;

	ofSetHexColor(0xFFFFFFFF);
	// disable for testing idle
	grayimg.draw(0, 0, ofGetWidth(), ofGetHeight());

	ofNoFill();

	ofSetHexColor(0xFFFF00FF);
	ofDrawRectRounded(facerect.x * xscale, facerect.y*yscale, facerect.width*xscale, facerect.height*yscale, 30.0);


	float underface_x = facerect.x * xscale;
	//float underface_y = min(float(facerect.y + facerect.height + 10.), ofGetHeight() - 30.);
	float underface_y = std::min(float(facerect.getBottom()*yscale + 20.0), float(ofGetHeight() - 30));
	switch (state.state) {
	case S_IDLE:
		//draw_idle();
		draw_idle1();
		break;

	case S_HELLO:
		msg.drawString("Hello!", 50, 100);
		break;

	case S_QUESTION:
		msg.drawString("Hello!", 50, 100);
		msg.drawString("May we take your picture?", 50, ofGetHeight() - 150);
		msg.drawString("(nod yes or no)", 50, ofGetHeight() - 75);
		break;

	case S_THREE:

		msg.drawString("Ready! 3...", underface_x, underface_y);
		break;

	case S_TWO:
		msg.drawString("Ready! 3... 2...", underface_x, underface_y);
		break;

	case S_ONE:
		msg.drawString("Ready! 3... 2... 1...", underface_x, underface_y);
		break;

	case S_CAPTURE:
		msg.drawString("Ready! 3... 2... 1... Pose!", underface_x, underface_y);
		break;

	case S_YES_IMG:
		msg.drawString("Thank you!", underface_x, underface_y);
		ofSetColor(255, 255, 255, 127);
		idle_image.draw(0, 0, ofGetWidth(), ofGetHeight());
		break;

	case S_NO_IMG:
		msg.drawString("OK, thanks anyway!", 50, ofGetHeight() - 100);
		break;
	}
}

void ofApp::init_idle() {
	//some path, may be absolute or relative to bin/data
	cout << "init idle\n";
	string path = "";
	ofDirectory imgs(path);
	//only show png files
	imgs.allowExt("png");
	//populate the directory object
	//imgs.listDir();

	imgs.getSorted();
	imgs.listDir();
	//	if (imgs.size() > 0) {
	//		cout << "loading color file " << imgs.getPath(imgs.size() - 1) << "\n";
	//		idle_image.load(imgs.getFile(imgs.size()-1));
	//	}	
	int img_count = 0;
	int num_dir_images = imgs.size();
	for (int i = 0; i < std::min(num_images, num_dir_images); i++) {
		// load most recent images
		int j = imgs.size() - i - 1;
		cout << "loading gray file " << imgs.getPath(j) << "\n";
		gray_images[i].load(imgs.getFile(j));
		gray_images[i].setImageType(OF_IMAGE_GRAYSCALE);
		img_count++;

		// parse rectangle out of file name
		ofRectangle r = gray_rects[i];
		unsigned int x, y, w, h;
		char timestamp[20];
		sscanf(imgs.getPath(j).c_str(), "%14sx%3uy%3uw%3uh%3u.png", timestamp, &x, &y, &w, &h);
		gray_rects[i].set(x, y, w, h);
		cout << "timestamp: " << timestamp << "\n";
		cout << "x: " << x << " y:" << y << "\n";
		cout << "w: " << w << " h:" << h << "\n";
	}
	num_images = img_count;

}



void ofApp::store_image() {

	time_t rawtime;
	time(&rawtime);
	struct tm *timeinfo;
	timeinfo = localtime(&rawtime);
	char timestamp[80];
	strftime(timestamp, 80, "%m-%d-%H-%M-%S", timeinfo);
	cout << timestamp << std::endl;
	ofRectangle r = facerect;
	char name[256];
	sprintf(name, "%sx%03dy%03dw%03dh%03d.png", timestamp, int(r.x), int(r.y), int(r.width), int(r.height));
	cout << "saving image: " << name;
	saveimg.save(name);
	idle_image.clone(saveimg);
	id++;

}

//--------------------------------------------------------------
void ofApp::draw_idle1() {
	// list of images in ofDirectory imgs, load and display
	// idle1: align faces & crossfade
	ofSetColor(255, 255, 255, 255);

	if (yes_flag) {
		ofSetColor(255, 255, 255, int(127 * (cos(0.5*state.time_elapsed()) + 1)));

		idle_image.draw(0, 0, ofGetWidth(), ofGetHeight());
	}

	ofSetColor(255, 255, 255, 127);
	//grayimg.draw(0, 0, ofGetWidth(), ofGetHeight());

	for (int i = 0; i < num_images; i++) {
		ofRectangle r = gray_rects[i];
		ofPoint fc = gray_rects[i].getCenter();
		ofPoint cp = ofPoint(ofGetWidth() / 2, ofGetHeight() / 2, 0.);

		//gray_images[i].drawSubsection(x, y, ss, ss, r.width, r.height, r.x, r.y);

		// draw image so faces are centered. Face center is at fc
		ofPushMatrix();
		ofSetColor(0, 255, 0, 255);
		// scale faces to take up 1/3 of screen
		float scale =  ofGetWidth() / (3*r.width);
		//scale = 1.0;
		ofScale(scale, scale, 1.0);
		ofTranslate((cp.x/scale) - fc.x, (cp.y/scale) - fc.y);
		ofRect(r);
		ofSetColor(255, 255, 255, 127);
		gray_images[i].draw(0, 0);
		//x += ss;
		ofPopMatrix();
	}
	return;

	// brady bunch
	int x = 0;
	int y = 0;
	int maxy = -1;
	for (int i = 0; i < num_images; i++) {
		//ofSetColor(255, 255, 255, int(127 * (sin(float(i)*0.2*ofGetElapsedTimef()) + 1.)));
		ofSetColor(255, 255, 255, 255);
		ofRectangle r = gray_rects[i];
		//gray_images[i].drawSubsection(x, y, ss, ss, r.width, r.height, r.x, r.y);
		gray_images[i].drawSubsection(x, y, r.width, r.height, r.x, r.y);
		//x += ss;
		if (r.height > maxy) {
			maxy = r.height;
		}
		x += r.width;
		if (x > ofGetWidth() - 150) {
			y += maxy;
			x = 0;
			maxy = -1;
		}
	}

}
//--------------------------------------------------------------
void ofApp::draw_idle() {
	// boring plain image crossfade
	// list of images in ofDirectory imgs, load and display
	ofSetColor(255, 255, 255, 255);
	for (int i = 0; i < num_images; i++) {
		//ofSetColor(255, 255, 255, int((100 / (i + 1)) * (sin(float(i)*0.2*ofGetElapsedTimef()) + 1.)));
		ofSetColor(255, 255, 255, int(100 * (sin(float(i)*0.2*ofGetElapsedTimef()) + 1. / (i + 1))));
		//ofSetColor(255, 255, 255, 100);
		gray_images[i].draw(0, 0, ofGetWidth(), ofGetHeight());
		//ofSetColor(0,255,0,255);
		//ofRect(gray_rects[i]);
	}

	if (yes_flag) {
		ofSetColor(255, 255, 255, int(127 * (cos(0.5*state.time_elapsed()) + 1)));

		idle_image.draw(0, 0, ofGetWidth(), ofGetHeight());
	}
	ofSetColor(255, 255, 255, 127);
	grayimg.draw(0, 0, ofGetWidth(), ofGetHeight());

	return;

	// brady bunch
	int x = 0;
	int y = 0;
	int maxy = -1;
	for (int i = 0; i < num_images; i++) {
		//ofSetColor(255, 255, 255, int(127 * (sin(float(i)*0.2*ofGetElapsedTimef()) + 1.)));
		ofSetColor(255, 255, 255, 255);
		ofRectangle r = gray_rects[i];
		//gray_images[i].drawSubsection(x, y, ss, ss, r.width, r.height, r.x, r.y);
		gray_images[i].drawSubsection(x, y, r.width, r.height, r.x, r.y);
		//x += ss;
		if (r.height > maxy) {
			maxy = r.height;
		}
		x += r.width;
		if (x > ofGetWidth() - 150) {
			y += maxy;
			x = 0;
			maxy = -1;
		}
	}

}

void ofApp::clean_exit(int signal) {
	// clean exit, signal handler
	cout << "Exit signal caught, bye!\n";
	ofExit();
}
void ofApp::keyPressed(int key) {
	if (key == 'x') {
		ofExit();
	}
	if (key == 's') {
		// test back-to-idle handling
		saveimg.setFromPixels(cam.getPixels());
		if (finder.size()) {
			saverect = finder.getObjectSmoothed(0);
			store_image();
		}
		init_idle();
	}

	if (key == 'p')
	{
		time_t rawtime;
		time(&rawtime);
		struct tm *timeinfo;
		timeinfo = localtime(&rawtime);
		//char buffer[80];
		//strftime(buffer, 80, "Now it's %I:%M%p.", timeinfo);
		char timestamp[80];
		strftime(timestamp, 80, "%m-%d-%H-%M-%S", timeinfo);
		//cout << "Current time is ::  " << ctime(&rawtime) << std::endl;
		cout << timestamp << std::endl;
	}
}

// State machine handling ------------------------------------------------------------
void StateMach::set(int next_state, float timeout) {
	state = next_state;
	timeout_time = timeout;
	// default no timeout
	if (timeout >= 0) {
		ofResetElapsedTimeCounter();
		reset_timer();
	}
	print();
}

void StateMach::reset_timer(void) {
	start_time = ofGetElapsedTimef();
}

float StateMach::time_elapsed(void) {
	return(ofGetElapsedTimef() - start_time);
}

// return the fraction of elapsed time that has passed 
float StateMach::frac_time_elapsed(void) {
	if (time_elapsed() > timeout_time) {
		return 1.0;
	}
	return(time_elapsed() / timeout_time);
}


bool StateMach::timeout(void) {
	if (timeout_time < 0)
		return(true);
	if (time_elapsed() > timeout_time)
		return(true);
	return(false);
}

void StateMach::print(void) {
	cout << "State ID:" << state << " ";
	switch (state) {
	case S_IDLE:
		cout << "State: IDLE\n";
		break;
	case S_HELLO:
		cout << "State: HELLO\n";
		break;
	case S_QUESTION:
		cout << "State: QUESTION\n";
		break;
	case S_THREE:
		cout << "State: THREE\n";
		break;
	case S_TWO:
		cout << "State: TWO\n";
		break;
	case S_ONE:
		cout << "State: ONE\n";
		break;
	case S_YES_COUNT:
		cout << "State: YES_COUNT\n";
		break;
	case S_CAPTURE:
		cout << "State: CAPTURE\n";
		break;
	case S_NO_IMG:
		cout << "State: NO_IMG\n";
		break;
	case S_YES_IMG:
		cout << "State: YES_IMG\n";
		break;
	default:
		cout << "unknown state\n";
		break;
	}
}
