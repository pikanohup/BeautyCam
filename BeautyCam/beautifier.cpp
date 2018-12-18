#include <algorithm>
#include "beautifier.h"
#include "constants.h"

Beautifier::Beautifier(const char *key, const char *secret) : faceppApi(Facepp(key, secret))
{
}

bool comp(const Point2i a, const Point2i b) { return a.y < b.y; }

void Beautifier::initialize(const char *filePath)
{
	ori = imread(filePath);
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

Mat Beautifier::slimFace(Mat src)
{
	Mat dst = src;

	if (landmarks[NOSE_BRIDGE].size() > 0) {
		Point2i center = landmarks[NOSE_BRIDGE][landmarks[NOSE_BRIDGE].size() / 2];
		if (landmarks[CONTOUR_LEFT].size() >= 2) {
			double radius = 2.5 * distance(landmarks[CONTOUR_LEFT][0], landmarks[CONTOUR_LEFT][1]);
			for (int i = 0; i < landmarks[CONTOUR_LEFT].size(); i++) dst = warper.localTranslationWarp(dst, landmarks[CONTOUR_LEFT][i].x, landmarks[CONTOUR_LEFT][i].y,
				center.x, center.y, radius);
		}
		if (landmarks[CONTOUR_RIGHT].size() >= 2) {
			double radius = 2.5 * distance(landmarks[CONTOUR_RIGHT][0], landmarks[CONTOUR_RIGHT][1]);
			for (int i = 0; i < landmarks[CONTOUR_RIGHT].size(); i++) dst = warper.localTranslationWarp(dst, landmarks[CONTOUR_RIGHT][i].x, landmarks[CONTOUR_RIGHT][i].y,
				center.x, center.y, radius);
		}

	}

	return dst;
}

Mat Beautifier::enlageEyes(Mat src)
{
	Mat dst = src;

	if (landmarks[LEFT_EYE_CENTER].size() && landmarks[LEFT_EYE_BOTTOM].size() && landmarks[LEFT_EYE_TOP].size()) {
		Point2i center = landmarks[LEFT_EYE_CENTER][0];
		double radius = distance(landmarks[LEFT_EYE_BOTTOM][0], landmarks[LEFT_EYE_TOP][0]) * 1.5;
		dst = warper.localScalingWarp(dst, center.x, center.y, radius, 2.0);
	}
	if (landmarks[RIGHT_EYE_CENTER].size() && landmarks[RIGHT_EYE_BOTTOM].size() && landmarks[RIGHT_EYE_TOP].size()) {
		Point2i center = landmarks[RIGHT_EYE_CENTER][0];
		double radius = distance(landmarks[RIGHT_EYE_BOTTOM][0], landmarks[RIGHT_EYE_TOP][0]) * 1.5;
		dst = warper.localScalingWarp(dst, center.x, center.y, radius, 1.0);
	}

	return dst;
}

Mat Beautifier::autoBeautify()
{
	Mat src = ori.clone();
	return enlageEyes(ori);
}
