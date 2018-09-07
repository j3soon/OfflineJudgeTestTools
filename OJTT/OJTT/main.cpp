#include <boost/program_options.hpp>
#include <iostream>
#include "config_data.hpp"
#include "filesystem_home.hpp"

static const std::string version = "alpha";

/// <summary>Should be called directly by system.</summary>
/// <param name="argc">Arguments count.</param>
/// <param name="argv">Array of arguments sent through call, executable filename is also in it.</param>
int main(int argc, char *argv[]) {
	ojtt::config_data data(version);
	// Read args.
	try {
		int ret = data.setup_args(argc, argv, std::cout);
		if (ret != 0)
			return ret;
	} catch (boost::program_options::error& e) {
		std::cout << e.what();
		return 1;
	}
	// Start testing.
	if (data.test_single) {
	} else {
	}
}
