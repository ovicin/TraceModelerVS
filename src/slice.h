#ifndef _SLICE
#define _SLICE

#include "ofMain.h"

#include "ofxSTLExporter.h"
#include "ofxDelaunay.h"
#include "ofxVectorMath.h"

class slice {
	
public:
	
	/** Constructor */
	slice(vector<ofPoint> * myPoints, vector<slice> * mySlices, int myZSpacing, int mySmoothing);		

	/** Destructor */
	virtual ~slice();	

	/** Initialize by normalizing the position of the points, set the z value and return a new copy */
	vector<ofPoint> initialize(vector<ofPoint> * points, int z);
	/** Initialise the normals */
	void initializeNormals();
	
	
	/** Set the z spacing */
	void setZ(int z);
	/** Smooth a set of points */
	void smooth();
	/** Calculate the length of set of points */
	double getLength();
	/** Double version of ofDist */
	double distance(double x1, double y1, double x2, double y2);
	/** Distribute the points at an evenly spaced distance from one another */
	void  distributePoints(int numberOfPoints);
	/** Calculate the normal of a triangle */
	ofPoint calculateNormal(ofPoint p1, ofPoint p2, ofPoint p3);
	/** Calculate the centroid of a triangle */
	ofPoint calculateCentroid(ofPoint p1, ofPoint p2, ofPoint p3);
	/** Triangulate the outline of this slice */
	void triangulate();
	/** Find if the given point is inside the given polygon */
	bool insidePolygon(ofPoint *polygon,int N, ofPoint p);
	/** Get the winding direction. True if the points are in a clockwise direction, else false */
	bool getWinding(vector<ofPoint> *points);
	
	
	/** Draw the slice outline */
	void drawOutline();
	/** Draw the strip between this and the next slice */
	void drawStrip();
	/** Draw the strip between this and the next slice into the stl exporter */
	void drawStrip(ofxSTLExporter * exporter);
	/** Draw the triangulated slice */
	void drawSlice();
	/** Draw the triangulated slice into the stl exporter */
	void drawSlice(ofxSTLExporter * exporter);
	
	
	vector<ofPoint> points;
	int index;
	vector<slice> * pSlices;

	vector<ofPoint> normals;
	vector<ofxSTLFacet> triangles;
	
	
};

#endif