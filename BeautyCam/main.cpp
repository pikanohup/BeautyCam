#include <iostream>
#include "facepp.h"

const char *filePath = "0171_01.jpg";

int main(int argc, const char * argv[]) {
	const char * key = "x-pq6flagmculJoKDhhuCDSIkWARj-jG";
	const char * secret = "UbpURpZq7jWRDDwk0K8gQPVZYuk6LT9t";

	Facepp faceppApi = Facepp(key, secret);
	faceppApi.detect(filePath);

	system("pause");

	return 0;
}