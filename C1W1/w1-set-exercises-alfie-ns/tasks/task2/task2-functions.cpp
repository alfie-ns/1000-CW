#include "task2-functions.h"

/////////////////////////////////////// DO NOT MODIFY THE FOLLOWING ///////////////////////////////////////

//This is a tested function to calculate and return the area of a polygon with corner coordinates in the array points.
// You may call this from within this file
// You cannot call this directly from other files
// Do NOT change this function
static double areaOfPolygon_v1(Point points[], int n) {
    double area = 0.0;
    int j = n - 1;
    for (int i = 0; i < n; i++) {
        area += (points[j].x + points[i].x) * (points[j].y - points[i].y);
        j = i;
    }
    return abs(area / 2.0);
}

//This is a tested function to calculates and return the length of a line between points p1 and p2
// You may call this from within this file
// You cannot call this directly from other files
// Do NOT change this function
static double distanceBetweenPoints(Point p1, Point p2)
{
    double xd = p1.x - p2.x;
    double yd = p1.y - p2.y;
    double d = sqrt(xd * xd + yd * yd); // Good ol' Pythogoras
    return d;
}

//Display a point in the terminal
// Do NOT change this function
void displayPoint(Point p)
{
    cout.precision(1);
    cout << std::fixed;
    cout << "(" << p.x << "," << p.y << ")" << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// YOUR SOLUTION STARTS HERE ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

// calculate area of polygon
double areaOfPolygon(vector<Point> points) {

    double area = 0.0; // initialise area to 0.0 as a starting point for accumulation in shoelace formula
    int n = points.size(); // n = size of points

    // shoelace formula
    for (int i = 0; i < n; i++) { // loop through all points in points vector
        int j = (i + 1) % n; // wrap around 
        area += (points[i].x * points[j].y) - (points[j].x * points[i].y); // lace x and y 
    }

    // take absolute value and divide by 2
    area = abs(area) / 2.0;

    return area;
}

// calculate circumference of polygon
double circumferenceOfPolygon(vector<Point> points)
{
    double circumference = 0.0; // init circumference to 0.0 as starting point for accumulation
    int n = points.size(); // init n = size of points in vector

    // for each point in points, wrap around and add up distance between points
    for (int i = 0; i < n; i++) {
		int j = (i + 1) % n; // wrap around, modulo operator used to 'wrap around' to first item in vector
		circumference += distanceBetweenPoints(points[i], points[j]); // accumulate distance between points to circumference
	}

    return circumference; // return circumference to caller
}

// read data points from file
vector<Point> readDataPoints(string strFile)
{
    // the vector where the final result will be stored
    vector<Point> points;

    // open file in read mode
    ifstream file(strFile);

    if (!file.is_open()) { // if file cannot be opened
		cout << "Error opening file " << strFile << endl;
		return points;
	}
    if (file.is_open()) { // if file is open

        Point point; // init point as Point class
	    string line; // init line as string class

		// while there are lines to read
        while (getline(file, line)) {

			double x, y; // init x and y as doubles
			stringstream ss(line); // init stringstream with each line in file

            if (ss >> x >> y) { // if stringstream can read x and y

				// set point x and y
				point.x = x;
				point.y = y;

				points.push_back(point); // push back point to end of points vector
			}
		}
	}

    return points;
}