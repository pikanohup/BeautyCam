#ifndef BEAUTIFIER_H
#define BEAUTIFIER_H

#include <iostream>
#include <opencv.hpp>
#include <unordered_map>
#include "facepp.h"
#include "warper.h"
#include "filter.h"

using namespace cv;
using namespace std;

class Beautifier {
private:
	Mat ori;
	Mat mask;

	unordered_map<string, vector<Point2i>> landmarks;

	Facepp faceppApi;
	Warper warper;
	Filter filter;

public:
	Beautifier(const char *key, const char *secret);

	void initialize(const char *filePath);
	void clear();
	Mat getOri();
	double distance(Point2i a, Point2i b);

	Mat slimFace(Mat src, double param);
	Mat enlageEyes(Mat src, double param);
	Mat whitenSkin(Mat src, double param, bool skined);
	Mat smoothenSkin(Mat src, double param, bool skined);

	Mat autoBeautify(Mat src);
};

#endif
