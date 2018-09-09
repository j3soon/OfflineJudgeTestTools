#include <boost/program_options.hpp>
#include <iostream>
#include "config_data.hpp"
#include "ojtt.hpp"
#include "compiler.hpp"

static const std::string version = "alpha";

int pause(int exit_code) {
	std::cout << "Press enter key to exit . . .";
	std::cin.get();
	return exit_code;
}

/// <summary>Should be called directly by system.</summary>
/// <param name="argc">Arguments count.</param>
/// <param name="argv">Array of arguments sent through call, executable filename is also in it.</param>
int main(int argc, char *argv[]) {
	ojtt::config_data data(version);
	int ret;
	// Read args.
	try {
		ret = data.setup_args(argc, argv, std::cout);
		if (ret != 0)
			return data.pause ? pause(ret) : ret;
	} catch (boost::program_options::error& e) {
		std::cout << e.what();
		return data.pause ? pause(ret) : ret;
	}
	// Start compiling.
	if (ojtt::compiler::compile(data.file, data.compile, std::cout) != 0)
		return data.pause ? pause(ret) : ret;
	if (!data.test_single) {
		if (ojtt::compiler::compile(data.diff_file, data.compile, std::cout) != 0)
			return data.pause ? pause(ret) : ret;
	}
	// Start testing.
	// Pause the terminal.
	return data.pause ? pause(0) : 0;
}
