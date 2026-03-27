#include "read.hpp" 
#include <fstream> 
#include <iostream> 
#include <stdexcept> 

static string readFile(const string& filePath){ 

	// reads file from entire buffer stream 
	ifstream file(filePath, ios::in | ios::binary); 

	if (!file) { 
		throw runtime_error("Couldn't open file: " + filePath); 
	} 

	string contents;
	// changes the contents size to the start until the end of buffer 
	file.seekg(0, ios::end); 
	contents.resize(file.tellg()); 
	
	// reads the whole contents again 
	file.seekg(0, ios::beg); 
	file.read(&contents[0], contents.size());

	file.close();

	return contents; 
} 
