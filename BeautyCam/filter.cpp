#include "filter.h"

Mat Filter::whiten(Mat src, int beta)
{
	Mat dst = src.clone();

	double logb = log(beta) / 255.0;

	int channels = dst.channels();
	for (int i = 0; i < dst.cols; i++) {
		for (int j = 0; j < dst.rows; j++) {
			for (int k = 0; k < channels; k++) {
				dst.at<Vec3b>(j, i)[k] = saturate_cast<uchar>(( log(dst.at<Vec3b>(j, i)[k] / 255.0 * (beta - 1) + 1) ) / logb);
			}
		}
	}

	return dst;
}

Mat Filter::smoothen(Mat src, int alpha, int beta, double opacity)
{
	Mat layer1;
	bilateralFilter(src, layer1, 5 * alpha, 12.5 * beta, 12.5 * beta);

	Mat layer2 = layer1 - src + 128;

	Mat layer3;
	GaussianBlur(layer2, layer3, Size(2 * beta - 1, 2 * beta - 1), 0, 0);

	Mat layer4 = src + 2 * layer3 - 255;

	Mat dst = (1.0 - opacity) * src + opacity * layer4;
	return dst;
}

Mat Filter::enhance(Mat src)
{
	Mat dst;
	GaussianBlur(src, dst, Size(0, 0), 9);
	addWeighted(src, 1.2, dst, -0.2, 0, dst);
	return dst;
}
