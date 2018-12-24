#include <iostream>
#include "constants.h"
#include "beautifier.h"
#include <opencv.hpp>

using namespace cv;

int main(int argc, const char * argv[]) {
	Beautifier bf = Beautifier(KEY, SECRET);
	char filePath[25] = "test/0007_01.jpg";
	bf.initialize(filePath);
	
	Mat dst = bf.autoBeautify();
	//imwrite("test/0171_01_after.jpg", dst);

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