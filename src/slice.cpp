#include "slice.h"

//////////////////////////////////////////////////////////////
// CONSTRUCTOR
//////////////////////////////////////////////////////////////

slice::slice(vector<ofPoint> * myPoints, vector<slice> * mySlices, int myZSpacing, int mySmoothing) {

	pSlices = mySlices;
	index = pSlices->size();
	
	int z = index * myZSpacing;
	// Create a normalized copy of the original input points
	points = initialize(myPoints, z);
	
	float length = getLength();
	//cout << "Length: " << length << endl;
	
	// Apply smoothing iteratively
	for(int j = 0; j < mySmoothing; j++){
		smooth();
	}
	
	// Finally add one last point to close the outline and form a loop
	//ofPoint lastPoint(points[0].x, points[0].y, points[0].z);
	points.push_back(points[0]);
	
	cout << "Z Number of Points: " << points.size() << endl;
	
	// The number of points to create for this slice
	int numberOfPoints;
	if(index > 0){
		// Use the number of points from the previous slice
		numberOfPoints = (*pSlices)[index - 1].points.size();
	} else {
		// Else use the number of points given
		numberOfPoints = points.size();
	}
	
	cout << index << " - Number of Points: " << numberOfPoints << endl;
	distributePoints(numberOfPoints);
	
	if(index > 0){
		cout << "Index: " << index << endl;
		initializeNormals();
	}
	
}


slice::~slice() {
}

//////////////////////////////////////////////////////////////
// DRAW
//////////////////////////////////////////////////////////////
	
void slice::drawOutline(){
	
	if(pSlices->size() == 1){

		glBegin(GL_LINE_LOOP);
		//glLineWidth(0.1);
		//glBegin(GL_POINTS);
		//glPointSize(5);
		for(std::vector<ofPoint>::size_type i = 0; i < points.size(); i++) {
			glVertex3f(points[i].x, points[i].y, points[i].z);
		}
		glEnd();
			
	}
	
	
//	// START & END POINTS
//	glPointSize(10);
//	glBegin(GL_POINTS);
//	// Yellow
//	ofSetColor(255, 255, 0);
//	glVertex3f(points[0].x, points[0].y, points[0].z);
//	// Red
//	ofSetColor(255, 0, 0);
//	int last = points.size() - 1;
//	glVertex3f(points[last].x, points[last].y, points[last].z);
//	glEnd();
}

void slice::drawStrip(){
	// If this is not the only slice
	if(index > 0){

		// Store a reference to the previous slice
		vector<ofPoint> * prevPoints = &((*pSlices)[index - 1].points);
		int size = points.size() - 1;
		
		
		//glLineWidth(0.5);
//		glBegin(GL_LINE_STRIP);
		
//		for(std::vector<ofPoint>::size_type i = 0; i < points.size(); i++) {
//			
//			ofPoint normal1 = calculateNormal(points[i], (*prevPoints)[i], points[next]);
//			
//			glVertex3f((*prevPoints)[i].x, (*prevPoints)[i].y, (*prevPoints)[i].z);
//			glVertex3f(points[i].x, points[i].y, points[i].z);
//			// Calculate the normal of each triangle?
//			//glNormal3f(0, 0, 0);
//		}
		
		for(std::vector<ofPoint>::size_type i = 0; i < size; i++) {
			
			
			
			
			int next = i + 1;
			int normal1 = i * 2;
			int normal2 = normal1 + 1;
			{
				// Draw the normal for debugging
//				glBegin(GL_LINES);
//				ofPoint centroid = calculateCentroid((*prevPoints)[i], points[i], (*prevPoints)[next]);
//				glVertex3f(centroid.x, centroid.y, centroid.z);
//				glVertex3f(centroid.x + normals[normal1].x, centroid.y + normals[normal1].y, centroid.z + normals[normal1].z);
//				glEnd();
				
				glBegin(GL_TRIANGLES);
				glNormal3f(normals[normal1].x, normals[normal1].y, normals[normal1].z);
				glVertex3f((*prevPoints)[i].x, (*prevPoints)[i].y, (*prevPoints)[i].z);
				glVertex3f(points[i].x, points[i].y, points[i].z);
				glVertex3f((*prevPoints)[next].x, (*prevPoints)[next].y, (*prevPoints)[next].z);
				glEnd();
			}
			
			{
				// Draw the normal for debugging
//				glBegin(GL_LINES);
//				ofPoint centroid = calculateCentroid((*prevPoints)[next], points[i], points[next]);
//				glVertex3f(centroid.x, centroid.y, centroid.z);
//				glVertex3f(centroid.x + normals[normal2].x, centroid.y + normals[normal2].y, centroid.z + normals[normal2].z);
//				glEnd();
				
				glBegin(GL_TRIANGLES);
				glNormal3f(normals[normal2].x, normals[normal2].y, normals[normal2].z);
				glVertex3f((*prevPoints)[next].x, (*prevPoints)[next].y, (*prevPoints)[next].z);
				glVertex3f(points[i].x, points[i].y, points[i].z);
				glVertex3f(points[next].x, points[next].y, points[next].z);	
				glEnd();				
			}
			
			
//			{
//				// Draw the normal for debugging
//				glBegin(GL_LINES);
//				ofPoint centroid = calculateCentroid(points[i], (*prevPoints)[i], (*prevPoints)[next]);
//				glVertex3f(centroid.x, centroid.y, centroid.z);
//				glVertex3f(centroid.x + normals[i].x, centroid.y + normals[i].y, centroid.z + normals[i].z);
//				//glVertex3f(normals[i].x, normals[i].y, normals[i].z);
//				glEnd();
//				
//				glBegin(GL_TRIANGLES);
//				glNormal3f(normals[i].x, normals[i].y, normals[i].z);
//				glVertex3f((*prevPoints)[i].x, (*prevPoints)[i].y, (*prevPoints)[i].z);
//				glVertex3f(points[i].x, points[i].y, points[i].z);
//				glVertex3f((*prevPoints)[next].x, (*prevPoints)[next].y, (*prevPoints)[next].z);
//				glEnd();
//			}			
//
//			{
//				// Draw the normal for debugging
//				glBegin(GL_LINES);
//				ofPoint centroid = calculateCentroid(points[i], (*prevPoints)[next], points[next]);
//				glVertex3f(centroid.x, centroid.y, centroid.z);
//				glVertex3f(centroid.x + normals[next].x, centroid.y + normals[next].y, centroid.z + normals[next].z);				
//				//glVertex3f(normals[i].x, normals[i].y, normals[i].z);
//				glEnd();
//				
//				glBegin(GL_TRIANGLES);
//				glNormal3f(normals[next].x, normals[next].y, normals[next].z);
//				glVertex3f((*prevPoints)[next].x, (*prevPoints)[next].y, (*prevPoints)[next].z);
//				glVertex3f(points[i].x, points[i].y, points[i].z);
//				glVertex3f(points[next].x, points[next].y, points[next].z);				
//				glEnd();				
//			}
			
		
		}
		

		
	}
	
//	ofPoint p0(0, 0, 0);
//	ofPoint p1(0, 100, 0);
//	ofPoint p2(100, 0, 0);		
//	ofPoint p3(100, 100, 0);
//	
//	ofPoint n0 = calculateNormal(p1, p0, p2);
//	ofPoint n1 = calculateNormal(p1, p2, p3);
//	
//	glBegin(GL_LINES);
//	glVertex3f(p0.x, p0.y, p0.z);
//	glVertex3f(p0.x + n0.x, p0.y + n0.y, p0.z + n0.z);
//	
//	glVertex3f(p3.x, p3.y, p3.z);
//	glVertex3f(p3.x + n1.x, p3.y + n1.y, p3.z + n1.z);
//	glEnd();
//	
//	glBegin(GL_TRIANGLES);
//	
//	glNormal3f(n0.x, n0.x, n0.z);
//	glVertex3f(p1.x, p1.y, p1.z);
//	glVertex3f(p0.x, p0.y, p0.z);
//	glVertex3f(p2.x, p2.y, p2.z);		
//	
//	glNormal3f(n1.x, n1.x, n1.z);
//	glVertex3f(p1.x, p1.y, p1.z);
//	glVertex3f(p2.x, p2.y, p2.z);		
//	glVertex3f(p3.x, p3.y, p3.z);
//	
//	glEnd();
//	
//	p0 = ofPoint(150, 0, 100);
//	p1 = ofPoint(250, 100, 100);
//	p2 = ofPoint(250, 0, 0);		
//	p3 = ofPoint(150, 100, 0);
//	
//	n0 = calculateNormal(p1, p0, p2);
//	n1 = calculateNormal(p1, p2, p3);
//	
//	glBegin(GL_LINES);
//	glVertex3f(p0.x, p0.y, p0.z);
//	glVertex3f(p0.x + n0.x, p0.y + n0.y, p0.z + n0.z);
//	
//	glVertex3f(p3.x, p3.y, p3.z);
//	glVertex3f(p3.x + n1.x, p3.y + n1.y, p3.z + n1.z);
//	glEnd();
//	
//	glBegin(GL_TRIANGLES);
//	
//	glNormal3f(n0.x, n0.x, n0.z);
//	glVertex3f(p1.x, p1.y, p1.z);
//	glVertex3f(p0.x, p0.y, p0.z);
//	glVertex3f(p2.x, p2.y, p2.z);		
//	
//	glNormal3f(n1.x, n1.x, n1.z);
//	glVertex3f(p1.x, p1.y, p1.z);
//	glVertex3f(p2.x, p2.y, p2.z);		
//	glVertex3f(p3.x, p3.y, p3.z);
//	
//	glEnd();
	

//	TEST SQUARE
//	ofPoint p0[5];
//	ofPoint p1[5];
//	
//	for(int i = 0; i < 5; i++){
//		p0[i].x = p1[i].x = i*25;
//		p0[i].y = p1[i].y = 0;
//		p0[i].z = 0;
//		p1[i].z = 100;
//		
//		cout << p0[i].x << " " << p0[i].y << " " << p0[i].z << endl;
//		cout << p1[i].x << " " << p1[i].y << " " << p1[i].z << endl;		
//	}
//	
//	
//	for(int i = 0; i < 4; i++){
//		int next = i + 1;
//		
//		ofPoint n0 = calculateNormal(p0[i], p1[i], p1[next]);
//		ofPoint n1 = calculateNormal(p0[i], p1[next], p0[next]);
//		
//		glBegin(GL_LINES);
//		glVertex3f(p0[i].x, p0[i].y, p0[i].z);
//		glVertex3f(p0[i].x + n0.x, p0[i].y + n0.y, p0[i].z + n0.z);
//		
//		glVertex3f(p0[i].x, p0[i].y, p0[i].z);
//		glVertex3f(p0[i].x + n1.x, p0[i].y + n1.y, p0[i].z + n1.z);
//		glEnd();
//		
//		glBegin(GL_TRIANGLES);
//		
//		glNormal3f(n0.x, n0.x, n0.z);
//		glVertex3f(p0[i].x, p0[i].y, p0[i].z);
//		glVertex3f(p1[i].x, p1[i].y, p1[i].z);
//		glVertex3f(p1[next].x, p1[next].y, p1[next].z);
//
//		glNormal3f(n1.x, n1.x, n1.z);
//		glVertex3f(p0[i].x, p0[i].y, p0[i].z);
//		glVertex3f(p1[next].x, p1[next].y, p1[next].z);
//		glVertex3f(p0[next].x, p0[next].y, p0[next].z);
//		
//		glEnd();	
//	}

	
	
	
	
}

void slice::drawStrip(ofxSTLExporter * exporter){
	// If the next slice has been defined
	if(index > 0){
		
		// Store a reference to the previous slice
		vector<ofPoint> * prevPoints = &((*pSlices)[index - 1].points);
		int size = points.size() - 1;
		ofPoint normal;
		
		for(std::vector<ofPoint>::size_type i = 0; i < size; i++) {

			int next = i + 1;
			int normal1 = i * 2;
			int normal2 = normal1 + 1;

			// STL FILE: 
			// The vertices of the triangles should be listed in counterclockwise order, as viewed from outside the surface. 
			exporter->addTriangle(points[i], (*prevPoints)[next], (*prevPoints)[i], normals[normal1]);
			exporter->addTriangle(points[i], points[next], (*prevPoints)[next], normals[normal2]);
			
		}
		
	}
}

void slice::drawSlice(){
	int size = triangles.size();
	if(size > 0){

		ofPoint normal;
		if(index == 0) {
			normal = ofPoint(0, 0, -1);
		} else {
			normal = ofPoint(0, 0, 1);
		}
		
		
		
		for(int i = 0; i < size; i++){

			// DEBUG
//			glBegin(GL_LINES);
//			glNormal3f(normal.x, normal.y, normal.z);
//			ofPoint centroid = calculateCentroid(triangles[i].vert1, triangles[i].vert2, triangles[i].vert3);
//			glVertex3f(centroid.x, centroid.y, centroid.z);
//			glVertex3f(centroid.x + normal.x, centroid.y + normal.y, centroid.z + normal.z);
//			glEnd();
			
				
//			if(index == 0) {
//				glBegin(GL_LINES);	
//				glVertex3f(triangles[i].vert1.x, triangles[i].vert1.y, triangles[i].vert1.z);
//				glVertex3f(triangles[i].vert1.x, triangles[i].vert1.y, triangles[i].vert1.z - 10);
//				glVertex3f(triangles[i].vert2.x, triangles[i].vert2.y, triangles[i].vert2.z);
//				glVertex3f(triangles[i].vert2.x, triangles[i].vert2.y, triangles[i].vert2.z - 20);
//				glVertex3f(triangles[i].vert3.x, triangles[i].vert3.y, triangles[i].vert3.z);
//				glVertex3f(triangles[i].vert3.x, triangles[i].vert3.y, triangles[i].vert3.z - 30);
//				glEnd();
//			}
					
//			glBegin(GL_LINE_STRIP);
//			glVertex3f(triangles[i].vert1.x, triangles[i].vert1.y, triangles[i].vert1.z);
//			glVertex3f(triangles[i].vert2.x, triangles[i].vert2.y, triangles[i].vert2.z);
//			glVertex3f(triangles[i].vert3.x, triangles[i].vert3.y, triangles[i].vert3.z);			
//			glVertex3f(triangles[i].vert1.x, triangles[i].vert1.y, triangles[i].vert1.z);				
//			glEnd();
			
			glBegin(GL_TRIANGLES);
			glVertex3f(triangles[i].vert1.x, triangles[i].vert1.y, triangles[i].vert1.z);
			glVertex3f(triangles[i].vert2.x, triangles[i].vert2.y, triangles[i].vert2.z);
			glVertex3f(triangles[i].vert3.x, triangles[i].vert3.y, triangles[i].vert3.z);			
			glEnd();			
		
		}
	}
}

void slice::drawSlice(ofxSTLExporter * exporter){
	int size = triangles.size();
	if(size > 0){
		ofPoint normal;
		if(index == 0) {
			normal = ofPoint(0, 0, -1);
		} else {
			normal = ofPoint(0, 0, 1);
		}
		for(int i = 0; i < size; i++){

			// Change the direction of the back slice faces
			// Counter clockwise when looking from the outside
			if(index == 0) {
				exporter->addTriangle(triangles[i].vert1, triangles[i].vert3, triangles[i].vert2, normal);
			} else {
				exporter->addTriangle(triangles[i].vert1, triangles[i].vert2, triangles[i].vert3, normal);
			}
			
//			cout << "Slice Normal: " << normal.x << " " << normal.y << " " << normal.z << " " << endl;
		}
	}
}

	
//////////////////////////////////////////////////////////////
// GEOMETRY
//////////////////////////////////////////////////////////////
	
vector<ofPoint> slice::initialize(vector<ofPoint> * points, int z){
	int size = points->size();
	
	vector<ofPoint> newPoints(size);
	for(std::vector<ofPoint>::size_type i = 0; i < size; i++) {
		ofPoint point((*points)[i].x - 160, (*points)[i].y - 120, z);
		newPoints[i] = point;
	}
	return newPoints;
	
	
// POINT REORDERING	from the closest point to zero
//	// Find the closest point to zero
//	int size = points->size();
//	int closestToZero = 0;
//	double closestDistance = DBL_MAX;
//	for(std::vector<ofPoint>::size_type i = 0; i < size; i++) {
//		double thisDistance = distance( 0, 0, (*points)[i].x, (*points)[i].y);
//		if(thisDistance < closestDistance){
//			closestDistance = thisDistance;
//			closestToZero = i;
//		}
//	}
//	
//	// Reorder the points starting with the one closest to zero
//	int index = closestToZero;
//	
//	vector<ofPoint> newPoints(size);
//	for(std::vector<ofPoint>::size_type i = 0; i < size; i++) {
//		ofPoint point((*points)[index].x - 160, (*points)[index].y - 120, z);
//		newPoints[i] = point;
//		index++;
//		if(index >= size){
//			index = 0;
//		}
//	}
//	return newPoints;
	
}
	
void slice::initializeNormals(){
	
	//vector<ofPoint> initialNormals;
	vector<ofPoint> * prevPoints = &((*pSlices)[index - 1].points);

	int size = points.size() - 1;
	for(std::vector<ofPoint>::size_type i = 0; i < size; i++) {		

		int next = i + 1;
		ofPoint normal1 = calculateNormal((*prevPoints)[i], points[i], (*prevPoints)[next]);			
		normals.push_back(normal1);
		ofPoint normal2 = calculateNormal((*prevPoints)[next], points[i], points[next]);
		normals.push_back(normal2);
		
	}
	
	
	
//	int size = points.size() - 1;
//	for(std::vector<ofPoint>::size_type i = 0; i < size; i++) {
//		
//		if(i % 2 == 0) {
//			normal = calculateNormal((*prevPoints)[i], points[i], (*prevPoints)[i + 1]);			
//		} else {
//			normal = calculateNormal((*prevPoints)[i], points[i - 1], points[i]);
//		}
//
//		normals.push_back(normal);
//	}
//	
//	// Add the last two normals
//	int prevPointsSize = (*prevPoints).size();
//	normal = calculateNormal((*prevPoints)[prevPointsSize - 2], points[points.size() - 2], (*prevPoints)[prevPointsSize - 1]);
//	normals.push_back(normal);
//	normal = calculateNormal((*prevPoints)[prevPointsSize - 1], points[points.size() - 2], points[points.size() - 1]);
//	normals.push_back(normal);

	
	
//	int intialSize = initialNormals.size();
//	int last = intialSize - 1;
//	for(std::vector<ofPoint>::size_type i = 0; i < intialSize; i++) {
//		ofPoint p1, p3;
//		ofPoint p2 = initialNormals[i];
//		if(i == 0){
//			p1 =  initialNormals.back();
//		} else {
//			p1 =  initialNormals[i-1];
//		}
//		
//		if(i == last){
//			p3 =  initialNormals[0];
//		} else {
//			p3 =  initialNormals[i+1];
//		}
//		
//		ofPoint averageP;
//		averageP.x = (p1.x + p2.x + p3.x) / 3;
//		averageP.y = (p1.y + p2.y + p3.y) / 3;
//		averageP.z = (p1.z + p2.z + p3.z) / 3;
//		normals.push_back(averageP);
//	}
}

void slice::setZ(int z){
	int size = points.size();
	for(std::vector<ofPoint>::size_type i = 0; i < size; i++) {
		points[i].z = z;
	}
	size = triangles.size();
	for(std::vector<ofxSTLFacet>::size_type i = 0; i < size; i++) {
		triangles[i].vert1.z = z;
		triangles[i].vert2.z = z;
		triangles[i].vert3.z = z;
	}
}

void slice::smooth(){
	
	int size = points.size();
	int last = size - 1;
	
	// Allocate enough space for all the points in a new vector
	vector<ofPoint> smoothedPoints(size);
	
	// FIRST
	// Average the 3 points
	float x = points[last].x * 0.25F + points[0].x * 0.5F + points[1].x * 0.25F;
	float y = points[last].y * 0.25F + points[0].y * 0.5F + points[1].y * 0.25F;
	ofPoint firstPoint(x, y, points[0].z);
	smoothedPoints[0] = firstPoint;
	
	for(std::vector<ofPoint>::size_type i = 1; i < last; i++) {
		// MIDDLE
		int prev = i - 1;
		int next = i + 1;
		// Average the 3 points
		x = points[prev].x * 0.25F + points[i].x * 0.5F + points[next].x * 0.25F;
		y = points[prev].y * 0.25F + points[i].y * 0.5F + points[next].y * 0.25F;
		ofPoint middlePoint(x, y, points[i].z);
		smoothedPoints[i] = middlePoint;
	}
	
	// Average the 3 points
	x = points[last - 1].x * 0.25F + points[last].x * 0.5F + points[0].x * 0.25F;
	y = points[last - 1].y * 0.25F + points[last].y * 0.5F + points[0].y * 0.25F;
	ofPoint lastPoint(x, y, points[last].z);
	smoothedPoints[last] = lastPoint;
	
	// Copy the smoothed points back to the original
	points = smoothedPoints;
	//points.assign( smoothedPoints.begin(), smoothedPoints.end() );

}

double slice::getLength(){
	int size = points.size();
	double total = 0;
	for(std::vector<ofPoint>::size_type i = 1; i < size; i++) {
		ofPoint p0 = points[i - 1];
		ofPoint p1 = points[i];
		total += distance(p0.x, p0.y, p1.x, p1.y);
	}
	return total;
}

double slice::distance(double x1, double y1, double x2, double y2) {
	return sqrt(double((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
}


void slice::distributePoints(int numberOfPoints){
	
	// The length of this set of points
	double length = getLength();
	// The desired distance between the points
	double regularGap = length / (numberOfPoints - 1);
	
	// The distance walked around the uneven points
	double irregularWalked = distance(points[0].x, points[0].y, points[1].x, points[1].y);
	// The distance stepped around the even distribution
	double regularWalked = regularGap;
	
	int size = points.size();
	vector<ofPoint> distributedPoints(numberOfPoints);
	// Add the first point
	distributedPoints[0] = points[0];
		
	int j = 1;
	
	for(std::vector<ofPoint>::size_type i = 2; i < size; i++) {
		ofPoint p0 = points[i - 1];
		ofPoint p1 = points[i];
		double irregularGap = distance(p0.x, p0.y, p1.x, p1.y);
		double tempIrregularWalked = irregularWalked + irregularGap;
		
		while (tempIrregularWalked >= regularWalked && j < numberOfPoints){
			// If we have walked past the step distance 
			// then add a point 
			
			// We need this distance from the previous irregular point to the next regular point
			double stepPercentage = (regularWalked - irregularWalked) / irregularGap;
			double xDist = p1.x - p0.x;
			double yDist = p1.y - p0.y;
			double x = p0.x + xDist * stepPercentage;
			double y = p0.y + yDist * stepPercentage;
			
			ofPoint point(x, y, p0.z);
			distributedPoints[j] = point;
			j++;
			
			regularWalked += regularGap;
		} 		
		irregularWalked = tempIrregularWalked;
	}
	
	// Add the last point
	ofPoint lastPoint = points[size - 1];
	distributedPoints[numberOfPoints - 1] = lastPoint;
	points = distributedPoints;
	
}

ofPoint slice::calculateNormal(ofPoint p1, ofPoint p2, ofPoint p3){
	
//	float vec1X = p2->x - p1->x;
//	float vec1Y = p2->y - p1->y;
//	float vec1Z = p2->z - p1->z;
//	
//	float vec2X = p3->x - p2->x;
//	float vec2Y = p3->y - p2->y;
//	float vec2Z = p3->z - p2->z;
	
	
//	float crossX = vec1Y * vec2Z - vec1Z * vec2Y;
//	float crossY = vec1Z * vec2X - vec1X * vec2Z;
//	float crossZ = vec1X * vec2Y - vec1Y * vec2X;
//	
//	float dist = sqrt(crossX * crossX + crossY * crossY + crossZ * crossZ);
//
//	return ofPoint(crossX/dist, crossY/dist, crossZ/dist);

	
//	ofxVec3f vec1 = p2 - p1; 
//	ofxVec3f vec2 = p3 - p1;
//	
//	vec1.normalize();
//	vec2.normalize();
//	
//	ofxVec3f normal = vec1.perpendicular(vec2); 
//	if(normal.x == -0){
//		normal.x = 0;
//	}
//	if(normal.y == -0){
//		normal.y = 0;
//	}
//	if(normal.z == -0){
//		normal.z = 0;
//	}
	
	
	
//	ofPoint u, v;
//	u.x = p2.x - p1.x;
//	u.y = p2.y - p1.y;
//	u.z = p2.z - p1.z;	
//	v.x = p3.x - p1.x;
//	v.y = p3.y - p1.y;
//	v.z = p3.z - p1.z;	
//	
//	ofPoint normal;
//	normal.x = (u.y * v.z) - (u.z * v.y);
//	normal.y = (u.z * v.x) - (u.x * v.z);
//	normal.z = (u.x * v.y) - (u.y * v.x);
	
	// Vector of each side
	ofxVec3f v1 = p3 - p2;
	ofxVec3f v2 = p1 - p2;
	ofxVec3f normal = v1.crossed(v2);
	normal.normalize();
	normal *= 10;
	
//	ofPoint centroid = calculateCentroid(p1, p2, p3);
//	normal += centroid;
	
//	cout << "p1 " << p1.x << ", " << p1.y << ", " << p1.z << endl;
//	cout << "p2 " << p2.x << ", " << p2.y << ", " << p2.z << endl;
//	cout << "p3 " << p3.x << ", " << p3.y << ", " << p3.z << endl;	
//	cout << "v1 " << v1.x << ", " << v1.y << ", " << v1.z << endl;	
//	cout << "v2 " << v2.x << ", " << v2.y << ", " << v2.z << endl;	
//	cout << "normal " << normal.x << ", " << normal.y << ", " << normal.z << endl;	
//	cout << endl;
	
	return normal;
}

ofPoint slice::calculateCentroid(ofPoint p1, ofPoint p2, ofPoint p3) { 
	ofPoint centroid; 
	centroid.x = p1.x + p2.x + p3.x; 
	centroid.y = p1.y + p2.y + p3.y; 
	centroid.z = p1.z + p2.z + p3.z; 
	
	centroid.x /= 3; 
	centroid.y /= 3; 
	centroid.z /= 3; 
	
	return centroid; 
} 

void slice::triangulate(){
	ofxDelaunay triangulator;
	int size = points.size();
	for(std::vector<ofPoint>::size_type i = 0; i < size; i++) {
		triangulator.addPoint(points[i].x, points[i].y, points[i].z);
	}
	triangulator.triangulate();
	
	//ofPoint lastPoint;
	
	int ntri = triangulator.getNumTriangles();
	if( ntri > 0){
		cout << "Triangulated! " <<  ntri << endl;
		//triangulator.outputTriangles();
		
		ITRIANGLE *v = triangulator.getTriangles();
		XYZ *p = triangulator.getPoints();
		
		for(int i = 0; i < ntri; i++){
			
			ofPoint t1 (p[v[i].p1].x, p[v[i].p1].y, p[v[i].p1].z);
			ofPoint t2 (p[v[i].p2].x, p[v[i].p2].y, p[v[i].p2].z);
			ofPoint t3 (p[v[i].p3].x, p[v[i].p3].y, p[v[i].p3].z);
			
			// Check if the centroid of this triangle is inside the outline
			ofPoint centroid = calculateCentroid(t1, t2, t3);
			bool inside = insidePolygon(&points[0], points.size(), centroid);
			// Add the triangle if inside
			if(inside){
				
				ofxSTLFacet facet;
				facet.vert1 = t1;
				facet.vert2 = t2;
				facet.vert3 = t3;

				vector<ofPoint> pts;
				pts.push_back(t1);
				pts.push_back(t2);
				pts.push_back(t3);
				
				bool clockwise = getWinding(&pts);
//				bool samePoint = (t1 == lastPoint);
//
//				if(samePoint) {
//					cout << "Equal to Last Point!" << endl;
//				}
//				if(clockwise) {
//					cout << "Polygon winding incorrect, ignoring this polygon..." << endl;
//				}
//				//cout << "Normal: " << normal.x << " " << normal.y << " " << normal.z << endl;

//				// Check that the polygon is ordered counter clockwise
				if(!clockwise) {
					triangles.push_back(facet);					
				} 
//				
//				lastPoint = t3;
			}
		}
		
		// Because the triangulation algorithm is not perfect...
		// Now we do a very slow check to make sure that there are no overlapping polygons
//		for(int i = 0; i < triangles.size(); i++) {
//			ofPoint centroid = calculateCentroid(triangles[i].vert1, triangles[i].vert2, triangles[i].vert3);
//			for(int j = triangles.size(); j > 0; j--) {
//				if(i != j) {
//					ofPoint pts[3];
//					pts[0] = triangles[j].vert1;
//					pts[1] = triangles[j].vert2;
//					pts[2] = triangles[j].vert3;
//					if(insidePolygon(&pts[0], 3, centroid)) {
//						
//						vector<ofPoint> ipts;
//						ipts.push_back(triangles[i].vert1);
//						ipts.push_back(triangles[i].vert2);
//						ipts.push_back(triangles[i].vert3);						
//						bool iclockwise = getWinding(&ipts);
//						
//						vector<ofPoint> jpts;
//						ipts.push_back(triangles[j].vert1);
//						ipts.push_back(triangles[j].vert2);
//						ipts.push_back(triangles[j].vert3);						
//						bool jclockwise = getWinding(&jpts);
//						
//						cout << "Overlapping Triangle Found!" << endl;
//						
//						// Delete the clockwise one
//						if( !iclockwise && jclockwise) {
//							triangles.erase(triangles.begin() + j);	
//						} else if( iclockwise && !jclockwise) {
//							triangles.erase(triangles.begin() + i);								
//						} else {
//							//triangles.erase(triangles.begin() + i);
//						}
//						
//						
//						
//					}
//				}
//			}
//			
//		}
		
		
	}
}

bool slice::insidePolygon(ofPoint *polygon, int N, ofPoint p) {
	int counter = 0;
	int i;
	double xinters;
	ofPoint p1,p2;
	
	p1 = polygon[0];
	for (i=1;i<=N;i++) {
		p2 = polygon[i % N];
		if (p.y > MIN(p1.y,p2.y)) {
			if (p.y <= MAX(p1.y,p2.y)) {
				if (p.x <= MAX(p1.x,p2.x)) {
					if (p1.y != p2.y) {
						xinters = (p.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
						if (p1.x == p2.x || p.x <= xinters)
							counter++;
					}
				}
			}
		}
		p1 = p2;
	}
	
	if (counter % 2 == 0)
		return false;
	else
		return true;
}

bool slice::getWinding(vector<ofPoint> *points) {
	float total = 0;
	for (int i = 0; i < points->size(); i++) {
		ofPoint thisPoint = (*points)[i];
		ofPoint prevPoint, nextPoint;
		int sizeMinusOne = points->size() - 1;
		if (i == 0) {
			prevPoint = (*points)[sizeMinusOne];
		} else {
			prevPoint = (*points)[i - 1];
		}
		if (i == sizeMinusOne) {
			nextPoint = (*points)[0];
		} else {
			nextPoint = (*points)[i + 1];
		}
		total += thisPoint.x * (nextPoint.y - prevPoint.y);
	}
	total /= 2;
	return (total > 0) ? true : false;
}
