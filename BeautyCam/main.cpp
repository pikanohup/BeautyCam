#include <iostream>
#include "constants.h"
#include "beautifier.h"
#include <opencv.hpp>
using namespace cv;

const char *filePath = "0171_01.jpg";

int main(int argc, const char * argv[]) {
	Beautifier bf = Beautifier(KEY, SECRET);
	bf.initialize(filePath);
	
	Mat dst = bf.autoBeautify();

#ifdef _DEBUG
	Mat src = bf.getOri();
	Mat comp = Mat(src.rows, 2 * src.cols, src.type());
	src.copyTo(comp.colRange(0, src.cols));
	dst.copyTo(comp.colRange(src.cols, 2 * src.cols));
	imshow("comp", comp);
	waitKey(0);
#endif

	return 0;
}