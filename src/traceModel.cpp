#include "traceModel.h"

bool showCapsOld = true;

//--------------------------------------------------------------
void traceModel::setup(){

	// GL Setup
	ofSetVerticalSync(true);
	ofBackground(200, 200, 200);
	glDisable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	displayList = glGenLists(1);
	updateRequested = true;

	glEnable(GL_COLOR_MATERIAL);
	//glEnable(GL_LIGHTING); //Enable lighting
	glEnable(GL_LIGHT0); //Enable light #0
	glEnable(GL_LIGHT1); //Enable light #1
	glEnable(GL_NORMALIZE); //Automatically normalize normals
	glShadeModel(GL_SMOOTH); //Enable smooth shading

	
	
	// Video
	videoWidth = 320;
	videoHeight = 240;
	halfVideoWidth = videoWidth / 2;
	halfVideoHeight = videoHeight / 2;
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(videoWidth, videoHeight);
	colorImg.allocate(videoWidth, videoHeight);
	grayImage.allocate(videoWidth, videoHeight);
	grayBg.allocate(videoWidth, videoHeight);
	grayDiff.allocate(videoWidth, videoHeight);
	minBlobSize = 100;
	maxBlobSize = (videoWidth * videoHeight) / 2;
	
	// View
	prev_x = 0;
	prev_y = 0;
	view_rotx = 0.0f;
    view_roty = 1.0f;
	view_rotz = 0.0f;
	halfWidth = ofGetWidth() * 0.5;
	halfHeight = ofGetHeight() * 0.5;
	
	// Interface
	bLearnBakground = true;
	videoSettings = false;
	threshold = 35;
	captureRate = 100;
	manualCapture = false;
	capture = false;
	smoothing = 5;
	zSpacing = 50;
	clear = false;
	loadDebug = false;
	exportModel = false;
	reset = false;
	zoom = 0.0;
	keyDown = -1;
	showCaps = showCapsOld;
	
	// gui.addTitle("A group");
	// gui.addToggle("myBool1", myBool1);
	gui.addButton("Video Settings", videoSettings);
	gui.addButton("Learn Background", bLearnBakground);
	gui.addSlider("Threshold", threshold, 0, 100);
	gui.addSlider("Capture Rate", captureRate, 1, 500);
	gui.addToggle("Manual Capture", manualCapture);
	gui.addButton("Capture", capture);
//	gui.addButton("Load Debug", loadDebug);
	gui.addButton("Clear Shape", clear);
	gui.addButton("Export Model", exportModel);
	gui.addSlider("Smoothing", smoothing, 0, 25); 
	gui.addSlider("Z Spacing", zSpacing, 1, 100);
	gui.addSlider("Zoom", zoom, -1000.0, 1000.0);
	gui.addToggle("Show Caps", showCaps);
	gui.addButton("Reset View", reset);
	gui.show();
	
}

//--------------------------------------------------------------
void traceModel::update(){
	
	// Open the video settings window
	if(videoSettings){
		vidGrabber.videoSettings();
		videoSettings = false;
	}
	// Clear the geometry
	if(clear){
		slices.clear();
		clear = false;
		updateRequested = true;
	}
	
	// Load the debug file
	if(loadDebug){
		slices.clear();
		vector<ofPoint> points = readDebugFile();
		slice newSlice(&points, &slices, zSpacing, smoothing);
		newSlice.triangulate();
		slices.push_back(newSlice);
		loadDebug = false;
		updateRequested = true;
	}
	
	// Export the model
	if(exportModel){
		exportModel = false;
		vector<ofxSTLFacet> facets;
		int size = slices.size();
		stlExporter.beginModel("Trace Model");
		for(int i = 0; i<size; i++){
			slices[i].drawStrip(&stlExporter);
			slices[i].drawSlice(&stlExporter);
		}
		//stlExporter.useASCIIFormat(true); //export as ASCII (default is binary)
		//stlExporter.saveModel("/Users/karlwillis/Desktop/testASCII.stl");
		stlExporter.useASCIIFormat(false); //export as binary
		stlExporter.saveModel("/Users/karlwillis/Desktop/testBin.stl");
		
		cout << "Model Saved" << endl;
	}
	
	if(showCaps != showCapsOld){
		updateRequested = true;
	}
	
	// Reset view
	if(reset){
		resetRotation();
		reset = false;
	}
    
	// CAPTURE
	bool autoCap = (!manualCapture && ofGetFrameNum() % captureRate == 0);
	bool manCap = (manualCapture && capture);
	bool cap = (autoCap || manCap);
	
	// CAMERA
	bool bNewFrame = false;
	vidGrabber.grabFrame();
	bNewFrame = vidGrabber.isFrameNew();

	if (bNewFrame){
        colorImg.setFromPixels(vidGrabber.getPixels(), videoWidth,videoHeight);
        grayImage = colorImg;
		if (bLearnBakground == true){
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}
		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);
		//if(cap) {
			// input image, min size, max size, number considered, find holes
			contourFinder.findContours(grayDiff, minBlobSize, maxBlobSize, 1, false);			
		//}
	}
	
	// If the zSpacing has changed
	if(zSpacing != zSpacingChange){
		for(std::vector< vector<ofPoint> >::size_type i = 0; i < slices.size(); i++) {
			slices[i].setZ(i * zSpacing);
		}
		zSpacingChange = zSpacing;
		updateRequested = true;
	}
		
	// NEW GEOMETRY
	for (int i = 0; i < contourFinder.nBlobs; i++){
		
		if(cap){
			//writeDebugFile(&contourFinder.blobs[i].pts);
			int size = slices.size();
			int z = size * zSpacing;
			slice newSlice(&contourFinder.blobs[i].pts, &slices, zSpacing, smoothing);
			// Triangulate the new slice
			newSlice.triangulate();
			// So long as this is not the first slice, clear the triangles
			if(size > 1){
				slices[size - 1].triangles.clear();
			}
			slices.push_back(newSlice);	
			updateRequested = true;
			
			cout << "Slice Points: " << newSlice.points.size() << " Normals:  " << newSlice.normals.size() << endl;
			
			capture = false;
		}
    }
	showCapsOld = showCaps;
}

//--------------------------------------------------------------
void traceModel::draw(){

	// Draw the video
	ofSetColor(0xffffff);
	colorImg.draw(0, 0);
	//grayImage.draw(0, 0);
	grayBg.draw(videoWidth, 0);
	grayDiff.draw(videoWidth + videoWidth, 0);

	gui.draw();

	// UPDATE THE DISPLAY LIST
	if(updateRequested){
		displayList = glGenLists(1);
		glNewList(displayList, GL_COMPILE);
		int size = slices.size();
		int last = size - 1;
		
		for(std::vector< vector<ofPoint> >::size_type i = 0; i < slices.size(); i++) {
			slices[i].drawOutline();
			slices[i].drawStrip();
			if(showCaps){
				if(i == 0 || i == last){
					slices[i].drawSlice();
				}
			}
			
			
		}
		glEndList();
		updateRequested = false;
	
	}

	setupLighting();
	
	// DRAW THE GEOMETRY
	glPushMatrix();
	{
		glTranslated(halfWidth + 100, halfHeight + 100, zoom);

		glRotatef(view_rotx, 1.0f, 0.0f, 0.0f);
		glRotatef(view_roty, 0.0f, 1.0f, 0.0f);
		glRotatef(view_rotz, 0.0f, 0.0f, 1.0f);
		
		ofSetColor(255, 0, 0, 255);
//		ofSetColor(50, 50, 50, 50);	
		glCallList(displayList);
	}
	glPopMatrix();		

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

}

void traceModel::writeDebugFile(vector<ofPoint> * points){
	ofstream debugFile ("/Users/karlwillis/Desktop/debug.txt");
	if (debugFile.is_open()){
		int size = points->size();
		for(std::vector<ofPoint>::size_type i = 0; i < size; i++) {
			ofPoint p = (*points)[i];
			debugFile << p.x << " " << p.y << " " <<  p.z << endl;
		}
		debugFile.close();	
		cout << "Debug File Written" << endl;
	}
}

vector<ofPoint> traceModel::readDebugFile(){
	ifstream debugFile("/Users/karlwillis/Desktop/zeroBug.txt");
	vector<ofPoint> points;
	string line;
	int xyz[3];
	
	if (debugFile.is_open()){

		string token;
		stringstream iss;
		istringstream intStream;
		while ( getline(debugFile, line) ) {
			iss << line;
			int i = 0;
			// For each of the three points xyz
			while ( getline(iss, token, ' ') ) {
				// Cast to to a string
				istringstream intStream(token);
				intStream >> xyz[i];
				if(i == 2){
					i = 0;
				} else {
					i++;
				}
			}
			ofPoint newPoint(xyz[0], xyz[1], xyz[2]);
			points.push_back(newPoint);
			iss.clear();
		}
		debugFile.close();
	}
	
	return points;
}

void traceModel::setupLighting(){
	
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	
	//Add ambient light
	GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color (0.2, 0.2, 0.2)
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	//Add positioned light
	GLfloat lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color (0.5, 0.5, 0.5)
	GLfloat lightPos0[] = {4.0f, 0.0f, 8.0f, 1.0f}; //Positioned at (4, 0, 8)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	//Add directed light
	GLfloat lightColor1[] = {0.5f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)
	//Coming from the direction (-1, 0.5, 0.5)
	GLfloat lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	
	
}

void traceModel::resetRotation() {
	view_rotx = 0.0f;
	view_roty = 1.0f;
	view_rotz = 0.0f;
}

void traceModel::rotate(int x, int y) {
	
	float xDiff = x - prev_x;
	float yDiff = y - prev_y;
	float thetaY = 360.0f * (xDiff * 0.005f);
	float thetaX = 360.0f * (yDiff * 0.005f);
	
	prev_x = x;
	prev_y = y;
	
	view_rotx += thetaX;
	view_roty += thetaY;
	
	//cout << "Theta:" << setw( 15 ) << thetaX << setw( 15 ) << thetaY << setw( 5 ) << "Rotate:" << setw( 15 ) << view_rotx << setw( 15 ) << view_roty << endl;
	
    if (view_rotx > 90) {
		view_rotx = 90;
	}
	if (view_rotx < -90) {
		view_rotx = -90;
	}
}

//--------------------------------------------------------------
void traceModel::keyPressed  (int key){
	keyDown = key;

	switch (key){
//		case ' ':
//			bLearnBakground = true;
//			break;
//		case '+':
//			threshold ++;
//			if (threshold > 255) threshold = 255;
//			break;
//		case '-':
//			threshold --;
//			if (threshold < 0) threshold = 0;
//			break;
//		case 's':
//			vidGrabber.videoSettings();
//			break;
		case OF_KEY_RETURN:
			capture = true;
			break;
			
	}
}

void traceModel::keyReleased (int key){
	keyDown = -1;
}

//--------------------------------------------------------------
void traceModel::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void traceModel::mouseDragged(int x, int y, int button){
	// RIGHT CLICK
	if(button == 2 || keyDown == ' '){
//		rotX = (ofGetWidth() * 0.5) - x;
//		rotY = (ofGetHeight() * 0.5) - y;
		rotate(x, y);
	}
}

//--------------------------------------------------------------
void traceModel::mousePressed(int x, int y, int button){
	if(button == 2){
		prev_x = x;
        prev_y = y;
	}
}

//--------------------------------------------------------------
void traceModel::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void traceModel::windowResized(int w, int h){
}

