#ifndef OJTT_H
#define OJTT_H

namespace ojtt {
	namespace filesystem {
		boost::filesystem::path home_directory_path();
	}
	namespace string {
		std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
	}
	namespace regex {
		std::string escape(const std::string& str);
	}
}
#endif