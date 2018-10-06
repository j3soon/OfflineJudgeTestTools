#include "ojtt.hpp"
#include <boost/regex.hpp>

#ifdef _WIN32
#pragma comment(lib, "psapi.lib")
#include <psapi.h>
boost::filesystem::path ojtt::filesystem::home_directory_path() {
	char* homedir;
	std::string home;
	if ((homedir = getenv("HOME")) == nullptr) {
		home = getenv("HOMEDRIVE");
		home += getenv("HOMEPATH");
	} else {
		home = homedir;
	}
	return boost::filesystem::path(home);
}

std::string ojtt::string::replaceAll(const std::string& str, const std::string& from, const std::string& to) {
	std::string s = str;
	if (from.empty())
		return s;
	size_t start_pos = 0;
	while ((start_pos = s.find(from, start_pos)) != std::string::npos) {
		s.replace(start_pos, from.length(), to);
		// In case 'to' contains 'from', like replacing 'x' with 'yx'.
		start_pos += to.length();
	}
	return s;
}

std::string ojtt::regex::escape(const std::string& str) {
	const boost::regex esc("[.^$|()\\[\\]{}*+?\\\\]");
	const std::string rep("\\\\&");
	return regex_replace(str, esc, rep, boost::match_default | boost::format_sed);
}

size_t ojtt::process::getPeakWorkingSetSize(const boost::process::child& child) {
	PROCESS_MEMORY_COUNTERS ppsmemCounters;
	if (!GetProcessMemoryInfo(child.native_handle(), &ppsmemCounters, sizeof(PROCESS_MEMORY_COUNTERS)))
		throw GetLastError();
	return ppsmemCounters.PeakWorkingSetSize;
}

uint64_t ojtt::process::getProcessExecutionTime(const boost::process::child& child) {
	FILETIME lpCreationTime, lpExitTime, lpKernelTime, lpUserTime;
	if (!GetProcessTimes(child.native_handle(), &lpCreationTime, &lpExitTime, &lpKernelTime, &lpUserTime))
		throw GetLastError();
	uint64_t creationTime = static_cast<uint64_t>(lpCreationTime.dwHighDateTime) << 32 | lpCreationTime.dwLowDateTime;
	uint64_t exitTime = static_cast<uint64_t>(lpExitTime.dwHighDateTime) << 32 | lpExitTime.dwLowDateTime;
	return exitTime - creationTime;
}

#else
/*#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

boost::filesystem::path ojtt::filesystem::home_directory_path() {
	const char *homedir;
	if ((homedir = getenv("HOME")) == NULL) {
		homedir = getpwuid(getuid())->pw_dir;
	}
	return boost::filesystem::path(home);
}*/
#endif