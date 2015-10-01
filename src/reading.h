#include<string>
#include<map>
class FileParser {
	public:
		FileParser(const std::string &filename);
		std::string getString(const std::string &key) const;
		double getDouble(const std::string &key) const;
		int getInt(const std::string &key) const;
	private:
		std::map<std::string,std::string> m;
};
const FileParser *readConfig(const std::string &filename);

