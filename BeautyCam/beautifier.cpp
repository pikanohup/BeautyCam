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
	mask = filter.skinMask(ori);

	cout << "Waiting for response from face++...\n";
	Json::Value value = faceppApi.detect(filePath);

	landmarks = faceppApi.extractLandmarks(value);
	for (auto iter = landmarks.begin(); iter != landmarks.end(); iter++) {
		sort(iter->second.begin(), iter->second.end(), comp);
	}
}

void Beautifier::clear()
{
	landmarks.clear();
}

Mat Beautifier::getOri()
{
	return ori.clone();
}

double Beautifier::distance(Point2i a, Point2i b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

Mat Beautifier::slimFace(Mat src, double param)
{
	Mat dst = src;

	if (landmarks[NOSE_BRIDGE].size() > 0) {
		int centerX = landmarks[NOSE_BRIDGE][landmarks[NOSE_BRIDGE].size() / 2].x;

		if (landmarks[CONTOUR_LEFT].size() >= 2) {
			double radius = param * distance(landmarks[CONTOUR_LEFT][0], landmarks[CONTOUR_LEFT][1]);
			for (int i = 0; i < landmarks[CONTOUR_LEFT].size(); i++) {
				dst = warper.localTranslationWarp(dst, landmarks[CONTOUR_LEFT][i].x, landmarks[CONTOUR_LEFT][i].y,
					centerX, landmarks[CONTOUR_LEFT][i].y, radius);
			}
		}
		if (landmarks[CONTOUR_RIGHT].size() >= 2) {
			double radius = param * distance(landmarks[CONTOUR_RIGHT][0], landmarks[CONTOUR_RIGHT][1]);
			for (int i = 0; i < landmarks[CONTOUR_RIGHT].size(); i++) {
				dst = warper.localTranslationWarp(dst, landmarks[CONTOUR_RIGHT][i].x, landmarks[CONTOUR_RIGHT][i].y,
					centerX, landmarks[CONTOUR_RIGHT][i].y, radius);
			}
		}
	}

	cout << " slim face: " << param << endl;
	return dst;
}

Mat Beautifier::enlageEyes(Mat src, double param)
{
	Mat dst = src;

	if (landmarks[LEFT_EYE_CENTER].size() && landmarks[LEFT_EYE_BOTTOM].size() && landmarks[LEFT_EYE_TOP].size()) {
		Point2i center = landmarks[LEFT_EYE_CENTER][0];
		double radius = distance(landmarks[LEFT_EYE_BOTTOM][0], landmarks[LEFT_EYE_TOP][0]) * 1.2 * param;
		dst = warper.localScalingWarp(dst, center.x, center.y, radius, param);
	}
	if (landmarks[RIGHT_EYE_CENTER].size() && landmarks[RIGHT_EYE_BOTTOM].size() && landmarks[RIGHT_EYE_TOP].size()) {
		Point2i center = landmarks[RIGHT_EYE_CENTER][0];
		double radius = distance(landmarks[RIGHT_EYE_BOTTOM][0], landmarks[RIGHT_EYE_TOP][0]) * 1.2 * param;
		dst = warper.localScalingWarp(dst, center.x, center.y, radius, param);
	}

	cout << " enlarge eyes: " << param << endl;
	return dst;
}

Mat Beautifier::whitenSkin(Mat src, double param, bool skined)
{
	Mat dst = src.clone();

	dst = filter.whiten(dst, param);
	if (skined) {
		dst = filter.blend(src, dst, mask);
		cout << " whiten: " << param << "; skin detection: true\n";
	}
	else {
		cout << " whiten: " << param << "; skin detection: false\n";
	}

	return dst;
}

Mat Beautifier::smoothenSkin(Mat src, double param, bool skined)
{
	Mat dst = src.clone();

	dst = filter.smoothen(dst, param, 3, 0.5);
	if (skined) {
		dst = filter.blend(src, dst, mask);
		cout << " smoothen: " << param << "; skin detection: true\n";
	}
	else {
		cout << " smoothen: " << param << "; skin detection: false\n";
	}

	return dst;
}

Mat Beautifier::autoBeautify(Mat src)
{
	Mat dst = src;

	dst = slimFace(dst, 2.7);
	dst = enlageEyes(dst, 1.1);
	dst = whitenSkin(dst, 5, true);
	dst = smoothenSkin(dst, 3, true);

	return dst;
}
