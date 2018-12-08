#include <cmath>
#include "warper.h"

void Warper::bilinearInsert(Mat &img, double ux, double uy, int x, int y)
{
	int channels = img.channels();
	int x1 = int(ux); int y1 = int(uy);
	int x2 = x1 + 1; int y2 = y1 + 1;
	for (int i = 0; i < channels; i++) {
		double p1 = img.at<Vec3b>(y1, x1)[i] * ((double)x2 - ux) * ((double)y2 - uy);
		double p2 = img.at<Vec3b>(y1, x2)[i] * (ux - (double)x1) * ((double)y2 - uy);
		double p3 = img.at<Vec3b>(y2, x1)[i] * ((double)x2 - ux) * (uy - (double)y1);
		double p4 = img.at<Vec3b>(y2, x2)[i] * (ux - (double)x1) * (uy - (double)y1);
		img.at<Vec3b>(y, x)[i] = p1 + p2 + p3 + p4;
	}
}

Mat Warper::localTranslationWarp(Mat src, int startX, int startY, int endX, int endY, double radius)
{
	Mat dst = src;

	double p2radius = radius * radius;
	int p2mc = (endX - startX) * (endX - startX) + (endY - startY) * (endY - startY);

	for (int i = 0; i < dst.cols; i++) {
		for (int j = 0; j < dst.rows; j++) {
			if (fabs(i - startX) > radius && fabs(j - startY) > radius) continue;

			int p2distance = (i - startX) * (i - startX) + (j - startY) * (j - startY);
			if (p2distance < p2radius) {
				double ratio = (p2radius - p2distance) / (p2radius - p2distance + p2mc);
				double p2ratio = ratio * ratio;

				double oriX = i - p2ratio * (endX - startX);
				double oriY = j - p2ratio * (endY - startY);

				bilinearInsert(dst, oriX, oriY, i, j);
			}
		}
	}
	return dst;
}
