#include <cmath>
#include "warper.h"

void Warper::bilinearInsert(Mat src, Mat &img, double ux, double uy, int x, int y)
{
	int channels = src.channels();
	int x1 = int(ux); int y1 = int(uy);
	int x2 = x1 + 1; int y2 = y1 + 1;
	for (int i = 0; i < channels; i++) {
		double p1 = src.at<Vec3b>(y1, x1)[i] * ((double)x2 - ux) * ((double)y2 - uy);
		double p2 = src.at<Vec3b>(y1, x2)[i] * (ux - (double)x1) * ((double)y2 - uy);
		double p3 = src.at<Vec3b>(y2, x1)[i] * ((double)x2 - ux) * (uy - (double)y1);
		double p4 = src.at<Vec3b>(y2, x2)[i] * (ux - (double)x1) * (uy - (double)y1);
		img.at<Vec3b>(y, x)[i] = p1 + p2 + p3 + p4;
	}
}

Mat Warper::localTranslationWarp(Mat src, int startX, int startY, int endX, int endY, double radius)
{
	Mat dst = src.clone();

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

				bilinearInsert(src, dst, oriX, oriY, i, j);
			}
		}
	}

	return dst;
}

Mat Warper::localScalingWarp(Mat src, int centerX, int centerY, double radius, double ratio)
{
	Mat dst = src.clone();

	for (int i = 0; i < dst.cols; i++) {
		for (int j = 0; j < dst.rows; j++) {
			if (fabs(i - centerX) > radius && fabs(j - centerY) > radius) continue;

			double distance = sqrt((i - centerX) * (i - centerX) + (j - centerY) * (j - centerY));
			if (distance < radius) {
				double alpha = 1.0 - pow(distance / radius - 1.0, 2.0) * ratio;

				double oriX = centerX + alpha * (i - centerX);
				double oriY = centerY + alpha * (j - centerY);

				bilinearInsert(src, dst, oriX, oriY, i, j);
			}
		}
	}

	return dst;
}
