#pragma once
#include <iostream>
using namespace std;
#include <map>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>
class InstrumentsMap
{
public:
	InstrumentsMap();
	~InstrumentsMap();
	void AddElement(string key, string value);
	void ReplaceOldInstru(string key, string &newStru);
	void ReadFileTable(string filename);
	vector<string> split(string str, string pattern);
private:
	map<string, string> instruMap;
};

