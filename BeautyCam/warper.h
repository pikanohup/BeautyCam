#ifndef WARPER_H
#define WARPER_H

#include <opencv.hpp>

using namespace cv;

class Warper {
public:
	void bilinearInsert(Mat &img, double ux, double uy, int x, int y);
	Mat localTranslationWarp(Mat src, int startX, int startY, int endX, int endY, double radius);
};

#endif
