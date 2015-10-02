#include"reading.h"
#include"stdio.h"
#include<fstream>
#include<map>
#include<string>
#include<vector>
using std::ifstream;
using std::map;
using std::string;
using std::vector;
const static string whitespaces(" \t\f\v\n\r");
FileParser::FileParser(const string &filename) {
	string s="in."+filename+".txt";
	ifstream f(s.c_str());
	string line;
	while (getline(f,line)) {
		size_t l1=line.find_first_not_of(whitespaces);
		if (l1==string::npos) {
			continue;
		}
		size_t r1=line.find_first_of(whitespaces,l1);
		if (r1==string::npos) {
			r1=line.length();
		}
		string key=line.substr(l1,r1-l1);
		string value;
		size_t l2=line.find_first_not_of(whitespaces,r1);
		size_t r2;
		if (l2==string::npos) {
			value="1";
		}
		else {
			r2=line.find_last_not_of(whitespaces)+1;
			value=line.substr(l2,r2-l2);
		}
		m[key]=value;
	}
}
string FileParser::getString(const string &key) const {
	return m.at(key);
}
int FileParser::getInt(const std::string &key) const {
	const char *p=m.at(key).c_str();
	int x;
	sscanf(p,"%d",&x);
	return x;
}
double FileParser::getDouble(const std::string &key) const {
	const char *p=m.at(key).c_str();
	double x;
	sscanf(p,"%lf",&x);
	return x;
}
vector<double> FileParser::getDoubleVector(const std::string &key) const {
	const char *p=m.at(key).c_str();
	vector<double> v;
	int n;
	double x;
	while (sscanf(p,"%lf%n",&x,&n)!=EOF) {
		v.push_back(x);
		p+=n;
	}
	return v;
}
class ConfigLib {
	public:
		ConfigLib();
		~ConfigLib();
		const FileParser *read(const string &filename);
	private:
		map<string,FileParser *> m;
};
ConfigLib::ConfigLib() {
}
ConfigLib::~ConfigLib() {
	for (auto it=m.begin();it!=m.end();it++) {
		delete it->second;
	}
}
const FileParser *ConfigLib::read(const string &filename) {
	if (m.find(filename)==m.end()) {
		m[filename]=new FileParser(filename);
	}
	return m.at(filename);
}
static ConfigLib configLib;
const FileParser *readConfig(const string &filename) {
	return configLib.read(filename);
}

