#ifndef FACEPP_H
#define FACEPP_H

#include <iostream>
#include <map>

using namespace std;

class Facepp {
private:
	const char *key;
	const char *secret;
public:
	Facepp(const char *key, const char *secret);

	void doPost(const char *URL, map<const char *, const char *> params);
	void detect(const char *filePath);
};

#endif