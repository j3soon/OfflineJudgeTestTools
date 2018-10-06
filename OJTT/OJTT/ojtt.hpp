#ifndef OJTT_H
#define OJTT_H
#include <boost/filesystem/path.hpp>
#include <boost/process.hpp>

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
	namespace process {
		size_t getPeakWorkingSetSize(const boost::process::child& child);
		// In 100 ns.
		uint64_t getProcessExecutionTime(const boost::process::child& child);
	}
}
#endif