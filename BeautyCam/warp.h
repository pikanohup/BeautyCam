#ifndef WARP_H
#define WARP_H

#include <opencv.hpp>

using namespace cv;

class Warp {
private:
	Mat src;

public:
	Warp(Mat src);
	Mat localTranslationWarp();
};

#endif
