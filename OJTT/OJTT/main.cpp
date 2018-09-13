#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include "config_data.hpp"
#include "ojtt.hpp"
#include "testing.hpp"

static const std::string version = "alpha";

int exit(int exit_code, bool pause) {
	if (pause) {
		std::cout << "Press enter key to exit . . .";
		std::cin.get();
	}
	return exit_code;
}

int test_single(int& ac, const ojtt::config_data& data) {
	namespace ot = ojtt::testing;
	namespace fs = boost::filesystem;
	int ret;
	long long exec_time;
	boost::uuids::random_generator generator;
	std::string expected_file = (fs::path(data.tmp_dir_uuid) / ("expected_file_" + boost::uuids::to_string(generator()))).string();
	std::string actual_file = (fs::path(data.tmp_dir_uuid) / ("actual_file_" + boost::uuids::to_string(generator()))).string();
	fs::ofstream expected_writer(expected_file);
	fs::ofstream actual_writer(actual_file);

	// Test all test cases.
	for (size_t i = 0; i < data.input_output.size(); i++) {
		const auto& io = data.input_output[i];
		bool error = false;
		std::string progress = (i + 1) + "/" + data.input_output.size();
		std::cout << "Testing: " << ": " << boost::filesystem::path(data.file).filename().string() << "\n";
		std::string input, output, actual_output;
		// Read input, output from file.
		if (ret = ot::read(io.first, input, data.file, data.tmp_dir, std::cout)) return 1;
		input = ot::universal_eol(input, data.universal_eol);
		if (ret = ot::read(io.second, output, data.file, data.tmp_dir, std::cout)) return 1;
		output = ot::universal_eol(output, data.universal_eol);
		// Deal with input.
		if (!data.file_input.empty()) {
			// Input should be saved to destination.
			if (ret = ot::overwrite(data.file_input, input, data.file, data.tmp_dir_uuid, std::cout)) return 1;
			if (data.diff_level == 1) {
				// Show on console.
				std::cout << "Input:\n" << input << "\n";
			} else if (data.diff_level == 2) {
				// Save to diff files.
				expected_writer << "Testing: " << ": " << boost::filesystem::path(data.file).filename().string() << "\n";
				expected_writer << "Input:\n" << input << "\n";
				actual_writer << "Testing: " << ": " << boost::filesystem::path(data.file).filename().string() << "\n";
				actual_writer << "Input:\n" << input << "\n";
			}
			input = "";
		}
		// Get actual output.
		if (data.diff_level == 2) {
			expected_writer << "Output:\n" << output << "\n";
			if (ret = ot::execute(data.execute, input, actual_output, data.file, data.tmp_dir, data.time_out, exec_time, std::cout, &actual_writer)) {
				continue;
			}
		} else {
			if (ret = ot::execute(data.execute, input, actual_output, data.file, data.tmp_dir, data.time_out, exec_time, std::cout)) {
				continue;
			}
		}
		if (!data.file_output.empty()) {
			// Output should be read from destination.
			if (ret = ot::read(io.first, actual_output, data.file, data.tmp_dir, std::cout)) return 1;
		}
		actual_output = ot::universal_eol(actual_output, data.universal_eol);
		// Compare 'output' and 'actual_output'.
		if (output == actual_output) {
			ac++;
			std::cout << "Result: Passed";
		} else {
			//Output diff.
			//TODO: Make code below follow a certain design pattern.
			if (data.diff_level == 0) {
				// Don't show.
			} else if (data.diff_level == 1) {
				// Show on console.
				std::cout << "Expected output:\n" << (data.eol.empty() ? output : ojtt::string::replaceAll(output, "\n", data.eol + "\n")) << "\n";
				std::cout << "Actual output:\n" << (data.eol.empty() ? actual_output : ojtt::string::replaceAll(actual_output, "\n", data.eol + "\n")) << "\n";
			}
			std::cout << "Result: Failed";
		}
		std::cout << " (" << exec_time << "ms)\n";
		if (data.diff_level == 2) {
			// Save to diff files.
			actual_writer << "Output:\n" << actual_output << "\n";
		}
	}
	if (data.diff_level == 2) {
		// Show in diff GUI.
		std::string _;
		if (ret = ot::execute(data.diff, "", _, expected_file, actual_file, data.time_out, exec_time, std::cout)) return 1;
	}
	return 0;
}

/// <summary>Should be called directly by system.</summary>
/// <param name="argc">Arguments count.</param>
/// <param name="argv">Array of arguments sent through call, executable filename is also in it.</param>
int main(int argc, char *argv[]) {
	namespace ot = ojtt::testing;
	ojtt::config_data data(version);
	//TODO: Change 'ret' to c++ try-catch syntax.
	int ret = 1;
	try {
		// Read args.
		if (ret = data.setup_args(argc, argv, std::cout)) return exit(ret, data.pause);
		// Compile.
		if (ret = ot::compile(data.compile, data.file, data.tmp_dir_uuid, std::cout)) return exit(ret, data.pause);
		if (data.test_single) {
			int ac = 0;
			if (test_single(ac, data))
				return exit(ret, data.pause);
			// Report result.
			if (ac == data.input_output.size())
				std::cout << ac << "/" << data.input_output.size() << " AC (All Accepted)\n";
			else
				std::cout << ac << "/" << data.input_output.size() << " WA (Wrong Answer)\n";
		} else {
			// Compile diff file.
			if (ret = ot::compile(data.compile, data.diff_file, data.tmp_dir_uuid, std::cout)) return exit(ret, data.pause);
			//TODO: use randomizer blah blah blah.
			throw;
		}
		// End.
		return exit(0, data.pause);
	} catch (boost::program_options::error& e) {
		std::cout << "Unknown error occurred, please contact the developer for help.";
		std::cout << e.what();
		return exit(ret, data.pause);
	}
}
