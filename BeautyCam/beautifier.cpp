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

bool Beautifier::isSkin(int r, int g, int b)
{
	return ((r > 95 && g > 40 && b > 20) &&
		(max(r, max(g, b)) - min(r, min(g, b)) > 15) &&
		(abs(r - g) > 15) &&
		(r > g && r > b))
		||
		((r > 220 && g > 210 && b > 170) &&
		(abs(r - g) <= 15) &&
		(r > b && g > b));
}

Mat Beautifier::skinMask(Mat src)
{
	Mat mask = Mat::zeros(src.size(), CV_8UC1);

	for (int i = 0; i < src.cols; i++) {
		for (int j = 0; j < src.rows; j++) {
			int r = src.at<Vec3b>(j, i)[2];
			int g = src.at<Vec3b>(j, i)[1];
			int b = src.at<Vec3b>(j, i)[0];
			if (isSkin(r, g, b)) mask.at<uchar>(j, i) = 255;
		}
	}

	Mat labels, stats, centroids;
	int nccomps = cv::connectedComponentsWithStats(mask, labels, stats, centroids);
	int max_area = 0, max_label;
	for (int i = 1; i < nccomps; i++) {
		int area = stats.at<int>(i, cv::CC_STAT_AREA);
		if (area > max_area) {
			max_area = area;
			max_label = i;
		}
	}
	for (int i = 0; i < src.cols; i++) {
		for (int j = 0; j < src.rows; j++) {
			if (labels.at<int>(j, i) != max_label) mask.at<uchar>(j, i) = 0;
		}
	}

	GaussianBlur(mask, mask, Size(9, 9), 5, 5);
	
	return mask;
}

Mat Beautifier::slimFace(Mat src)
{
	Mat dst = src;

	if (landmarks[NOSE_BRIDGE].size() > 0) {
		int centerX = landmarks[NOSE_BRIDGE][landmarks[NOSE_BRIDGE].size() / 2].x;

		if (landmarks[CONTOUR_LEFT].size() >= 2) {
			double radius = 2.7 * distance(landmarks[CONTOUR_LEFT][0], landmarks[CONTOUR_LEFT][1]);
			for (int i = 0; i < landmarks[CONTOUR_LEFT].size(); i++) {
				dst = warper.localTranslationWarp(dst, landmarks[CONTOUR_LEFT][i].x, landmarks[CONTOUR_LEFT][i].y,
					centerX, landmarks[CONTOUR_LEFT][i].y, radius);
			}
		}
		if (landmarks[CONTOUR_RIGHT].size() >= 2) {
			double radius = 2.7 * distance(landmarks[CONTOUR_RIGHT][0], landmarks[CONTOUR_RIGHT][1]);
			for (int i = 0; i < landmarks[CONTOUR_RIGHT].size(); i++) {
				dst = warper.localTranslationWarp(dst, landmarks[CONTOUR_RIGHT][i].x, landmarks[CONTOUR_RIGHT][i].y,
					centerX, landmarks[CONTOUR_RIGHT][i].y, radius);
			}
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
		dst = warper.localScalingWarp(dst, center.x, center.y, radius, 1.1);
	}
	if (landmarks[RIGHT_EYE_CENTER].size() && landmarks[RIGHT_EYE_BOTTOM].size() && landmarks[RIGHT_EYE_TOP].size()) {
		Point2i center = landmarks[RIGHT_EYE_CENTER][0];
		double radius = distance(landmarks[RIGHT_EYE_BOTTOM][0], landmarks[RIGHT_EYE_TOP][0]) * 1.5;
		dst = warper.localScalingWarp(dst, center.x, center.y, radius, 1.1);
	}

	return dst;
}

Mat Beautifier::beautifySkin(Mat src)
{
	Mat tmp = src.clone();
	Mat mask = skinMask(src);
	Mat dst(src.size(), src.type());

	tmp = filter.whiten(tmp, 7);
	tmp = filter.smoothen(tmp, 2, 2, 0.5);
	//tmp = filter.enhance(tmp);

	int channels = src.channels();
	for (int i = 0; i < src.cols; i++) {
		for (int j = 0; j < src.rows; j++) {
			int flag = mask.at<uchar>(j, i);
			if (flag == 255) {
				dst.at<Vec3b>(j, i) = tmp.at<Vec3b>(j, i);
			}
			else if (flag == 0) {
				dst.at<Vec3b>(j, i) = src.at<Vec3b>(j, i);
			}
			else {
				double w = (double)flag / 255.0;
				for (int k = 0; k < channels; k++) {
					dst.at<Vec3b>(j, i)[k] = w * tmp.at<Vec3b>(j, i)[k] + (1.0 - w) * src.at<Vec3b>(j, i)[k];
				}
			}
		}
	}

	return dst;
}

Mat Beautifier::autoBeautify()
{
	Mat dst = ori.clone();

	dst = slimFace(dst);
	dst = enlageEyes(dst);
	dst = beautifySkin(dst);

	return dst;
}
