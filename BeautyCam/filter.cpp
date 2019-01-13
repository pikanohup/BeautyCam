#include "filter.h"

bool Filter::isSkin(int r, int g, int b)
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

Mat Filter::skinMask(Mat src)
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
	for (int i = 0; i < nccomps; i++) {
		int area = stats.at<int>(i, CC_STAT_AREA);
		if (area > max_area && stats.at<int>(i, CC_STAT_LEFT)) {
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

Mat Filter::blend(Mat src, Mat tmp, Mat mask)
{
	Mat dst(src.size(), src.type());

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
