#pragma once

#include <vector>
#include <string>
#include <fstream>

using namespace std;

static std::vector<char> loadText(std::string file)
{
	ifstream ifs(file.c_str(), ios::binary | ios::ate);
	ifstream::pos_type pos = ifs.tellg();

	std::vector<char> result(pos);

	ifs.seekg(0, ios::beg);
	ifs.read(&result[0], pos);

	return result;
}