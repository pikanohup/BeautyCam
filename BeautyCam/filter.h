#ifndef FILTER_H
#define FILTER_H

#include <opencv.hpp>

using namespace cv;

class Filter {
public:
	bool isSkin(int r, int g, int b);
	Mat skinMask(Mat src);
	Mat blend(Mat src, Mat tmp, Mat mask);

	Mat whiten(Mat src, int beta);
	Mat smoothen(Mat src, int alpha, int beta, double opacity);
	Mat enhance(Mat src);
};

#endif