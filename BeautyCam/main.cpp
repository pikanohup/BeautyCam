#include <iostream>
#include <cstring>
#include "constants.h"
#include "beautifier.h"
#include <opencv.hpp>

using namespace cv;
using namespace std;

#define equal(x, y) (strcmp((x), (y))==0)

int main(int argc, const char * argv[]) {
	Beautifier bf = Beautifier(KEY, SECRET);
	bf.initialize(argv[1]);

	Mat dst = bf.getOri();
	if (argc == 3) {
		dst = bf.autoBeautify(dst);
	}
	else {
		for (int i = 3; i < argc; i+=2) {
			if (equal(argv[i], "-f"))  dst = bf.slimFace(dst, strtod(argv[i + 1], NULL));
			else if (equal(argv[i], "-e"))  dst = bf.enlageEyes(dst, strtod(argv[i + 1], NULL));
			else if (equal(argv[i], "-w"))  dst = bf.whitenSkin(dst, strtod(argv[i + 1], NULL), false);
			else if (equal(argv[i], "-w--skin"))  dst = bf.whitenSkin(dst, strtod(argv[i + 1], NULL), true);
			else if (equal(argv[i], "-s"))  dst = bf.smoothenSkin(dst, strtod(argv[i + 1], NULL), false);
			else if (equal(argv[i], "-s--skin")) dst = bf.smoothenSkin(dst, strtod(argv[i + 1], NULL), true);
		}
	}

	imwrite(argv[2], dst);

	Mat src = bf.getOri();
	Mat comp = Mat(src.rows, 2 * src.cols, src.type());
	src.copyTo(comp.colRange(0, src.cols));
	dst.copyTo(comp.colRange(src.cols, 2 * src.cols));
	imshow("compare", comp);
	waitKey(0);

	return 0;
}