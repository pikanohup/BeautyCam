#include <sstream>
#include <curl/curl.h>
#include "facepp.h"

using namespace std;

Facepp::Facepp(const char *key, const char *secret) : key(key), secret(secret)
{
}

static size_t call_back(char *ptr, size_t size, size_t nmemb, void *content)
{
	string data(ptr, size * nmemb);
	*((stringstream*)content) << data << endl;

	return size * nmemb;
}

Json::Value Facepp::doPost(const char *URL, map<const char *, const char *> params) {

	map<const char*, const char*>::iterator iter;

	struct curl_httppost *post = NULL;
	struct curl_httppost *last = NULL;

	// initialize
	curl_global_init(CURL_GLOBAL_ALL);

	for (iter = params.begin(); iter != params.end(); iter++) {
		const char * key = iter->first;
		const char * value = iter->second;
		if (0 == strcmp("image_file", key)
			|| 0 == strcmp("image_file1", key)
			|| 0 == strcmp("image_file2", key)
			|| 0 == strcmp("template_file", key)
			|| 0 == strcmp("merge_file", key)) {
			curl_formadd(&post, &last,
				CURLFORM_COPYNAME, key,
				CURLFORM_FILE, value,
				CURLFORM_END);
		}
		else {
			curl_formadd(&post, &last,
				CURLFORM_COPYNAME, key,
				CURLFORM_COPYCONTENTS, value,
				CURLFORM_END);
		}
	}

	stringstream response;
	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(curl, CURLOPT_URL, URL);
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &call_back);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	// execute
	CURLcode rescode = curl_easy_perform(curl);
	if (CURLE_OK != rescode) {
		fprintf(stderr, " \n\n -------Request failed------- \n %s \n\n ", curl_easy_strerror(rescode));
	}

	// formate to json
	Json::Value value;
	Json::Reader reader;
	reader.parse(response.str(), value);

#ifdef _DEBUG
	Json::StyledWriter writer;	
	string result = writer.write(value);
	cout << " \n\n -------------Response------------ \n\n " << result << endl;
#endif

	// release resources
	curl_easy_cleanup(curl);
	curl_formfree(post);

	return value;
};

Json::Value Facepp::detect(const char *filePath) {

	if (NULL == filePath) {
		fprintf(stderr, "\n\n-------Request failed-------\n %s \n\n", "file path can not be empty !");
		return Json::Value("");
	}

	const char *URL = "https://api-cn.faceplusplus.com/facepp/v3/detect";

	map<const char *, const char *> params;
	params.insert(map<const char *, const char *>::value_type("api_key", key));
	params.insert(map<const char *, const char *>::value_type("api_secret", secret));
	params.insert(map<const char *, const char *>::value_type("image_file", filePath));
	params.insert(map<const char *, const char *>::value_type("return_landmark", "1"));
	// params.insert(map<const char *, const char *>::value_type("return_attributes", "beauty"));

	return doPost(URL, params);
}

unordered_map<string, vector<cv::Point2i>> Facepp::extractLandmarks(Json::Value raw)
{	
	unordered_map<string, vector<cv::Point2i>> landmarks;
	Json::Value raw_face = raw["faces"][0]["landmark"];
	if (!raw_face.size()) {
		return landmarks;
	}
	
	Json::Value::Members keys = raw_face.getMemberNames();
	for (auto iter = keys.begin(); iter != keys.end(); iter++) {
		string key = *iter;

		Json::Value::Members coords = raw_face[key].getMemberNames();
		if (coords.size() != 2) continue;
		int x = raw_face[key][coords[0]].asInt();
		int y = raw_face[key][coords[1]].asInt();
		cv::Point2i point(x, y);

		int pos = key.find("\n");
		if (pos != -1) {
			key.erase(pos, 1);
		}
		pos = key.size() - 1;
		if (isdigit(key[pos])) {
			key.erase(pos, 1);
		}
		
		landmarks[key].push_back(point);
	}

	return landmarks;
}