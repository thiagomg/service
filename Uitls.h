#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <initializer_list>
#include <algorithm>
#include <unordered_set>

class Utils {
public:

	template<typename T>
	static std::basic_string<T> getDirName(const std::basic_string<T> &filePath, T dirSep = ' ')
	{
		size_t pos = 0;
		if (dirSep == ' ') {
			pos = filePath.rfind('\\');
			if (pos == std::string::npos) {
				pos = filePath.rfind('/');
			}
		}
		else {
			pos = filePath.rfind(dirSep);
		}

		if (pos == std::string::npos || pos == 0)
			return filePath;

		return filePath.substr(0, pos);
	}

	template<typename T>
	static bool fileExists(const std::basic_string<T> &filePath)
	{
		struct __stat64 buf;
		int ret = _tstat64(filePath.c_str(), &buf);
		return ret == 0;
	}

	//string
	template<typename T>
	static bool endsWith(const std::basic_string<T> &buf, T c)
	{
		if (buf.empty()) return false;
		return buf.rbegin() == c;
	}

	//string
	template<typename T>
	static bool endsWith(const std::basic_string<T> &buf, const std::initializer_list<T> l)
	{
		if (buf.empty()) return false;
		auto it = buf.rbegin();
		for (auto c : l) {
			if (*it == c)
				return true;
		}
		return false;
	}

	template<typename T>
	static T getSeparator(const std::basic_string<T> &buf, std::unordered_set<T> l)
	{
		for(auto c : buf) {
			if (l.find(c) != l.end())
				return c;
		}
		return *(l.begin());
	}


};
