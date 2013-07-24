#ifndef _TRACE_MODEL_APP
#define _TRACE_MODEL_APP

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxSimpleGuiToo.h"
#include "ofxSTLExporter.h"

#include "slice.h"

#include <iostream>
#include <fstream>

class traceModel : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased  (int key);	
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

		/** Write the points to a debug file */
		void writeDebugFile(vector<ofPoint> * points);
		/** Read the points from a debug file */	
		vector<ofPoint> readDebugFile();
	
		/** Reset the rotation */
		void resetRotation();
		/** Rotate the scene */
		void rotate(int x, int y);
	
		/** Set up the Open GL Lighting */
		void setupLighting();
	
		// VIDEO
		ofVideoGrabber 		vidGrabber;
		ofxCvColorImage		colorImg;
		ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;
		ofxCvContourFinder 	contourFinder;
		int videoWidth, videoHeight, halfVideoWidth, halfVideoHeight;
		int minBlobSize;
		int maxBlobSize;
	
		// VIEW
		float view_rotx; 
		float view_roty;
		float view_rotz;
		int prev_x,    prev_y;
		int halfWidth;
		int halfHeight;
		GLuint displayList;
		bool updateRequested;

	
		// INTERFACE
		int 				threshold;
		int					smoothing;
		int					zSpacing;
		int					zSpacingChange;
		bool				bLearnBakground;
		bool				videoSettings;
		bool				clear;
		bool				loadDebug;
		bool				reset;
		float				zoom;
		int					captureRate;
		bool				manualCapture;
		bool				capture;
		bool				exportModel;
		int					keyDown;
		bool				showCaps;
	
		// GEOMETRY
		vector<slice> slices;
		ofxSTLExporter stlExporter;
	
	
};

#endif
