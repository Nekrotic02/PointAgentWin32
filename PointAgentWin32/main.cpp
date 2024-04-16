#define CURL_STATICLIB
#include <curl\curl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct ResponseData
{
	std::string data;
};

size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	struct ResponseData* mem = (struct ResponseData*)userp;

	mem->data.append((char*)contents, realsize); 

	return realsize;
}

std::string readFileContents(const std::string& filename)
{
	std::ifstream file(filename); 
	std::string contents; 

	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			contents += line + "\n";
		}
		file.close();
	}
	else
	{
		std::cerr << "Unable to open file: " << filename << std::endl;
	}
	// Remove the trailing newline character, if present
	if (!contents.empty() && contents.back() == '\n') {
		contents.pop_back();
	}

	return contents;
}


int main()

{
	std::string Team1Client1API = "0de7dc34d37098e7a3eca9c7f087e56b";
	std::string MachineID = "Team1Client1";
	CURL* curl; 
	CURLcode res;

	// Initial winsock
	curl_global_init(CURL_GLOBAL_ALL); 

	// get curl handle
	curl = curl_easy_init();

	if (curl) {

		// Filename Declaration
		std::string filename = "C:\\Users\\Nekrotic\\Desktop\\test.txt";

		// Retrieve Contents of file
		std::string fileContents = readFileContents(filename);

		// set target URL
		curl_easy_setopt(curl, CURLOPT_URL, "http://172.16.1.24:5000/api/post_data");
#
		std::string jsonData = "{\"MachineID\": \"" + MachineID + "\", \"TeamID\": \"" + fileContents + "\"}";

		// Specify the POST data
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

		// Set Custom Header for API 
		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, ("X-API-Key: " + Team1Client1API).c_str());
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		// Initialise Response Data 
		struct ResponseData responseData; 
		responseData.data = "";

		// Write callback function 
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback); 

		// Pass response data struct to the callback function 
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

		// Perform the request
		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else
		{
			// print response 
			std::cout << "Response received:\n" << responseData.data << std::endl; 
		}
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return 0;
}