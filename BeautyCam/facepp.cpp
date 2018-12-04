#include <sstream>
#include <curl/curl.h>
#include <json/json.h>
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

void Facepp::doPost(const char *URL, map<const char *, const char *> params) {

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
	Json::StyledWriter writer;
	reader.parse(response.str(), value);
	string result = writer.write(value);

	cout << " \n\n -------------Response------------ \n\n "
		 << result
		 << " \n\n -------------Response------------ \n\n ";

	// release resources
	curl_easy_cleanup(curl);
	curl_formfree(post);
};

void Facepp::detect(const char *filePath) {

	if (NULL == filePath) {
		fprintf(stderr, "\n\n-------Request failed-------\n %s \n\n", "file path can not be empty !");
		return;
	}

	const char *URL = "https://api-cn.faceplusplus.com/facepp/v3/detect";

	map<const char *, const char *> params;
	params.insert(map<const char *, const char *>::value_type("api_key", key));
	params.insert(map<const char *, const char *>::value_type("api_secret", secret));
	params.insert(map<const char *, const char *>::value_type("image_file", filePath));
	doPost(URL, params);
}