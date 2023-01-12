#include "utilities.hpp"

#include <sstream>

using namespace std;

vector<string> split(const string &s, char delim) 
{
	vector<string> result;
	istringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		result.push_back(item);
	}
	return result;
}
