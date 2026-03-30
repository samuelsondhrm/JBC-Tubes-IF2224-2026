#include "file.hpp" 
#include <fstream> 
#include <iostream> 
#include <stdexcept> 

string FileUtil::readFile(const string& filePath){ 

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


 int FileUtil::writeFile(const string& filePath, const string& buffer){ 
	ofstream file(filePath); 

	if (!file.is_open()){ 
        	throw runtime_error("Cannot open file for writing: " + filePath);
	} 
	
	file << buffer; 
	file.close(); 
	return 0; 
} 
