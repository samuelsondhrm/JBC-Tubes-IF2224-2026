#pragma once 
#include <string> 
using namespace std;
class FileUtil{ 
	public: 
		static string readFile(const string& filePath);
		static int writeFile(const string& filePath, const string& buffer);
};

