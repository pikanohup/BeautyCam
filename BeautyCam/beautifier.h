#ifndef BEAUTIFIER_H
#define BEAUTIFIER_H

#include <iostream>
#include <opencv.hpp>
#include <map>
#include "facepp.h"

using namespace cv;
using namespace std;

class Beautifier {
private:
	Mat src;
	map<string, vector<Point2i>> landmarks;
public:
	Facepp faceppApi;

public:
	Beautifier(const char *key, const char *secret);
	void initialize();
	void clear();
};

#endif
