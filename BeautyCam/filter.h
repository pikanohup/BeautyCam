#ifndef FILTER_H
#define FILTER_H

#include <opencv.hpp>

using namespace cv;

class Filter {
public:
	Mat whiten(Mat src, int beta);
	Mat smoothen(Mat src, int alpha, int beta, double opacity);
};

#endif