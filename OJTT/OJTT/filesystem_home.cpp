#include "filesystem_home.hpp"

#ifdef _WIN32
boost::filesystem::path ojtt::filesystem::home_directory_path() {
	char* homedir;
	std::string home;
	if ((homedir = getenv("HOME")) == nullptr) {
		home = getenv("HOMEDRIVE");
		home += getenv("HOMEPATH");
	}
	else {
		home = homedir;
	}
	return boost::filesystem::path(home);
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