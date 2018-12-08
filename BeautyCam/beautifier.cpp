#include <algorithm>
#include "beautifier.h"
#include "constants.h"

Beautifier::Beautifier(const char *key, const char *secret) : faceppApi(Facepp(key, secret))
{
}

bool comp(const Point2i a, const Point2i b) { return a.y < b.y; }

void Beautifier::initialize(const char *filePath)
{
	src = imread(filePath);
	//landmarks = faceppApi.extractLandmarks(faceppApi.detect(filePath));
	std::ifstream is;
	is.open("face.json", std::ios::binary);
	Json::Reader reader;
	Json::Value value;
	reader.parse(is, value);
	landmarks = faceppApi.extractLandmarks(value);

	for (auto iter = landmarks.begin(); iter != landmarks.end(); iter++) {
		sort(iter->second.begin(), iter->second.end(), comp);
	}
}

void Beautifier::clear()
{
	landmarks.clear();
}

double Beautifier::distance(Point2i a, Point2i b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

Mat Beautifier::slimFace()
{
	Mat dst = src.clone();

	if (landmarks[CONTOUR_LEFT].size() >= 2) {
		double radius = 1.7 * distance(landmarks[CONTOUR_LEFT][0], landmarks[CONTOUR_LEFT][1]);
		for (int i = 0; i < landmarks[CONTOUR_LEFT].size(); i++) dst = warper.localTranslationWarp(dst, landmarks[CONTOUR_LEFT][i].x, landmarks[CONTOUR_LEFT][i].y,
			landmarks[NOSE_TIP][0].x, landmarks[NOSE_TIP][0].y, radius);
	}
	if (landmarks[CONTOUR_RIGHT].size() >= 2) {
		double radius = 1.7 * distance(landmarks[CONTOUR_RIGHT][0], landmarks[CONTOUR_RIGHT][1]);
		for (int i = 0; i < landmarks[CONTOUR_RIGHT].size(); i++) dst = warper.localTranslationWarp(dst, landmarks[CONTOUR_RIGHT][i].x, landmarks[CONTOUR_RIGHT][i].y,
			landmarks[NOSE_TIP][0].x, landmarks[NOSE_TIP][0].y, radius);
	}

	return dst;
}
