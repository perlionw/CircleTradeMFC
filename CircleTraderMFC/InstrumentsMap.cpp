#include "stdafx.h"
#include "InstrumentsMap.h"


InstrumentsMap::InstrumentsMap()
{
	instruMap.clear();
}


InstrumentsMap::~InstrumentsMap()
{
}

void InstrumentsMap::AddElement(string key, string value)
{
	instruMap[key] = value;
}

void InstrumentsMap::ReplaceOldInstru(string key, string & newStru)
{
	map<string, string>::iterator it = instruMap.begin();
	for (; it != instruMap.end(); ++it)
	{
		if (!strcmp(key.c_str(), it->first.c_str()))
		{
			newStru = it->second;
		}
	}
}

vector<string> InstrumentsMap::split(string str, string pattern)
{
	vector<string> ret;
	if (pattern.empty()) return ret;
	size_t start = 0, index = str.find_first_of(pattern, 0);
	while (index != str.npos)
	{
		if (start != index)
			ret.push_back(str.substr(start, index - start));
		start = index + 1;
		index = str.find_first_of(pattern, start);
	}
	if (!str.substr(start).empty())
		ret.push_back(str.substr(start));
	return ret;
}

void InstrumentsMap::ReadFileTable(string filename)
{
	ifstream in;
	string infoInstru;
	string oldInstru;
	string newInstru;
	in.open(filename);
	while (in >> infoInstru)
	{
		vector<string> instruVect = split(infoInstru, "&&");
		oldInstru = instruVect.at(0);
		newInstru = instruVect.at(1);
		AddElement(oldInstru, newInstru);
	}
	in.close();
}
