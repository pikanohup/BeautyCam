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
	//imwrite("eye.jpg", dst);
	imshow("eye", dst);
	waitKey(0);

	return 0;
}