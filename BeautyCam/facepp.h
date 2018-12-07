#ifndef FACEPP_H
#define FACEPP_H

#include <iostream>
#include <map>
#include <json/json.h>
#include <opencv.hpp>

using namespace std;

class Facepp {
private:
	const char *key;
	const char *secret;
public:
	Facepp(const char *key, const char *secret);

	Json::Value doPost(const char *URL, map<const char *, const char *> params);
	Json::Value detect(const char *filePath);
	map<string, vector<cv::Point2i>> extractLandmarks(Json::Value raw);
};

#endif