#ifndef BEAUTIFIER_H
#define BEAUTIFIER_H

#include <iostream>
#include <opencv.hpp>
#include <unordered_map>
#include "warper.h"
#include "facepp.h"

using namespace cv;
using namespace std;

class Beautifier {
private:
	Mat ori;
	unordered_map<string, vector<Point2i>> landmarks;
	Facepp faceppApi;
	Warper warper;

public:
	Beautifier(const char *key, const char *secret);
	void initialize(const char *filePath);
	void clear();
	double distance(Point2i a, Point2i b);
	Mat slimFace(Mat src);
	Mat enlageEyes(Mat src);
	Mat autoBeautify();
};

#endif
