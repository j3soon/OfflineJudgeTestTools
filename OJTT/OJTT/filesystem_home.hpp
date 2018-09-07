#ifndef OJTT_FILESYSTEM
#define OJTT_FILESYSTEM
#include <boost/filesystem/path.hpp>

namespace ojtt {
	namespace filesystem {
		boost::filesystem::path home_directory_path();
	}
}
#endif