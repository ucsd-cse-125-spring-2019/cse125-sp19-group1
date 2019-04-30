#pragma once
#include <vector>
#include <utility>
#include <string>
#include <sstream>

class StringParser
{
public:
	
	// Returns vector of pairs where each vector element is a key-value pair of strings
	static std::vector<std::pair<std::string, std::string>> parseKeyValueString(const char * data);

	static std::vector<std::vector<int>> parse2DIntArrayString(std::string data);
};