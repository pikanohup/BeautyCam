#include <iostream>
#include "beautifier.h"

const char *filePath = "0171_01.jpg";

int main(int argc, const char * argv[]) {
	const char *key = "x-pq6flagmculJoKDhhuCDSIkWARj-jG";
	const char *secret = "UbpURpZq7jWRDDwk0K8gQPVZYuk6LT9t";

	Beautifier bf = Beautifier(key, secret);
	std::ifstream is;
	is.open("face.json", std::ios::binary);
	Json::Reader reader;
	Json::Value value;
	reader.parse(is, value);
	map<string, vector<cv::Point2i>> mp = bf.faceppApi.extractLandmarks(/*bf.faceppApi.detect(filePath)*/value);

	system("pause");

	return 0;
}