#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include "config_data.hpp"
#include "ojtt.hpp"
#include "testing.hpp"

static const std::string version = "v0.1";

int exit(int exit_code, bool pause) {
	if (pause) {
		std::cout << "Press enter key to exit . . .";
		std::cin.get();
	}
	return exit_code;
}

//TODO: Reuse the same code of the 2 functions below.

int test_single(int& ac, const ojtt::config_data& data) {
	namespace ot = ojtt::testing;
	namespace fs = boost::filesystem;
	int ret;
	size_t i = 0;
	long long exec_time;
	boost::uuids::random_generator generator;
	std::string expected_file = (fs::path(data.tmp_dir_uuid) / ("expected_file_" + boost::uuids::to_string(generator()))).string();
	std::string actual_file = (fs::path(data.tmp_dir_uuid) / ("actual_file_" + boost::uuids::to_string(generator()))).string();
	fs::ofstream expected_writer;
	fs::ofstream actual_writer;
	if (data.diff_level == 2) {
		expected_writer.open(expected_file);
		actual_writer.open(actual_file);
		expected_writer << "Expected Output File.\n";
		actual_writer << "Actual Output File.\n";
	}

	// Test all test cases.
	for (auto io : data.input_output) {
		i++;
		std::string progress = i + "/" + data.input_output.size();
		std::cout << "Testing: " << io.first << "\n";
		std::string input, output, original_input, actual_output;
		// Read input, output from file.
		if (ret = ot::read(io.first, input, data.file, data.tmp_dir, data.output_file, std::cout)) return 1;
		input = ot::universal_eol(input, data.universal_eol);
		if (ret = ot::read(io.second, output, data.file, data.tmp_dir, data.output_file, std::cout)) return 1;
		output = ot::universal_eol(output, data.universal_eol);
		if (data.diff_level == 2) {
			// Save to diff files.
			expected_writer << "Testing: " << io.first << "\n";
			expected_writer << "Input:\n" << input << "\n";
			actual_writer << "Testing: " << io.first << "\n";
			actual_writer << "Input:\n" << input << "\n";
		}
		original_input = input;
		// Deal with input.
		if (!data.file_input.empty()) {
			// Input should be saved to destination.
			if (ret = ot::overwrite(data.file_input, input, data.file, data.tmp_dir_uuid, data.output_file, std::cout)) return 1;
			input = "";
		}
		// Get actual output.
		if (data.diff_level == 2) {
			if (ret = ot::execute(data.execute, input, actual_output, data.file, data.tmp_dir_uuid, data.output_file, data.time_out, exec_time, std::cout, &actual_writer)) {
				continue;
			}
		} else {
			if (ret = ot::execute(data.execute, input, actual_output, data.file, data.tmp_dir_uuid, data.output_file, data.time_out, exec_time, std::cout)) {
				continue;
			}
		}
		if (!data.file_output.empty()) {
			// Output should be read from destination.
			if (ret = ot::read(data.file_output, actual_output, data.file, data.tmp_dir_uuid, data.output_file, std::cout)) return 1;
		}
		//TODO: Should universal_eol be after write to file?
		// Line below can prevent doubled new-line problem.
		actual_output = ot::universal_eol(actual_output, data.universal_eol);
		if (data.diff_level == 2) {
			expected_writer << "Output:\n" << output << "\n";
			actual_writer << "Output:\n" << actual_output << "\n";
		}
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
				std::cout << "Input:\n" << original_input << "\n";
				std::cout << "Expected output:\n" << (data.eol.empty() ? output : ojtt::string::replaceAll(output, "\n", data.eol + "\n")) << "\n";
				std::cout << "Actual output:\n" << (data.eol.empty() ? actual_output : ojtt::string::replaceAll(actual_output, "\n", data.eol + "\n")) << "\n";
			}
			std::cout << "Result: Failed";
		}
		std::cout << " (" << exec_time << "ms)\n";
	}
	expected_writer.close();
	actual_writer.close();
	if (data.diff_level == 2 && ac != data.input_output.size()) {
		// Show in diff GUI.
		std::string _;
		if (ret = ot::execute(data.diff, "", _, expected_file, actual_file, "", data.time_out, exec_time, std::cout)) return 1;
	}
	return 0;
}

int test_double(const ojtt::config_data& data) {
	namespace ot = ojtt::testing;
	namespace fs = boost::filesystem;
	int ret;
	long long _;
	boost::uuids::random_generator generator;
	std::string file1 = (fs::path(data.tmp_dir_uuid) / ("file1_" + boost::uuids::to_string(generator()))).string();
	std::string file2 = (fs::path(data.tmp_dir_uuid) / ("file2_" + boost::uuids::to_string(generator()))).string();
	fs::ofstream file1_writer;
	fs::ofstream file2_writer;
	std::string input, original_input, output1, output2;
	if (data.diff_level == 2) {
		file1_writer.open(file1);
		file2_writer.open(file2);
		file1_writer << "File1 Output File.\n";
		file1_writer << "File path: " << data.file << "\n";
		file2_writer << "File2 Output File.\n";
		file2_writer << "File path: " << data.diff_file << "\n";
	}
	std::cout << "Exhaustive Testing: \n";
	std::chrono::time_point<std::chrono::system_clock> start_clk = std::chrono::system_clock::now();
	unsigned long long iterations = 0;
	bool input_empty, output_empty;
	input_empty = output_empty = false;
	while (true) {
		iterations++;
		output1 = output2 = input = "";
		//Get input.
		if (ret = ot::execute(data.execute, "", input, data.input_randomizer, data.tmp_dir_uuid, data.output_file, data.time_out, _, std::cout))
			return 1;
		if (!data.file_random.empty()) {
			// Random output should be read from destination.
			if (ret = ot::read(data.file_random, input, data.file, data.tmp_dir_uuid, data.output_file, std::cout))
				return 1;
		}
		original_input = input;
		if (!input_empty && input.empty()) {
			std::cout << "Warning: Randomizer input is empty.\n";
			std::cout << "Press enter to ignore . . .";
			std::cin.get();
			input_empty = true;
		}
		// Deal with input.
		if (!data.file_input.empty()) {
			// Input should be saved to destination.
			if (ret = ot::overwrite(data.file_input, input, data.file, data.tmp_dir_uuid, data.output_file, std::cout)) return 1;
			input = "";
		}
		// Get actual output1.
		if (ret = ot::execute(data.execute, input, output1, data.file, data.tmp_dir_uuid, data.output_file, data.time_out, _, std::cout))
			break;
		if (!data.file_output.empty()) {
			// Output should be read from destination.
			if (ret = ot::read(data.file_output, output1, data.file, data.tmp_dir_uuid, data.output_file, std::cout))
				break;
		}
		output1 = ot::universal_eol(output1, data.universal_eol);
		// Get actual output2.
		if (ret = ot::execute(data.execute, input, output2, data.diff_file, data.tmp_dir_uuid, data.output_file, data.time_out, _, std::cout))
			break;
		if (!data.file_output.empty()) {
			// Output should be read from destination.
			if (ret = ot::read(data.file_output, output2, data.diff_file, data.tmp_dir_uuid, data.output_file, std::cout))
				break;
		}
		output2 = ot::universal_eol(output2, data.universal_eol);
		if (output1 != output2)
			break;
		if (!output_empty && output1.empty()) {
			std::cout << "Warning: Both files' output is empty.\n";
			std::cout << "Press enter to ignore . . .";
			std::cin.get();
			output_empty = true;
		}
		if (iterations % data.time_log == 0) {
			std::chrono::time_point<std::chrono::system_clock> end_clk = std::chrono::system_clock::now();
			auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_clk - start_clk);
			std::cout << "Iterated " << iterations << " times.\n";
			std::cout << "Time elapsed: " << milliseconds.count() << "ms (" << (milliseconds.count() / iterations) << " ms/iteration)\n";
		}
	}
	std::chrono::time_point<std::chrono::system_clock> end_clk = std::chrono::system_clock::now();
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_clk - start_clk);
	std::cout << "Found disagreeing output after " << iterations << " iterations.\n";
	std::cout << "Time total elapsed: " << milliseconds.count() << "ms (" << (milliseconds.count() / iterations) << " ms/iteration)\n";
	if (data.diff_level == 1) {
		// Show on console.
		std::cout << "Input:\n" << original_input << "\n";
		std::cout << "File1 path: " << data.file << "\n";
		std::cout << "File1 output:\n" << (data.eol.empty() ? output1 : ojtt::string::replaceAll(output1, "\n", data.eol + "\n")) << "\n";
		std::cout << "File2 path: " << data.diff_file << "\n";
		std::cout << "File2 output:\n" << (data.eol.empty() ? output2 : ojtt::string::replaceAll(output2, "\n", data.eol + "\n")) << "\n";
	} else if (data.diff_level == 2) {
		file1_writer << "Input:\n" << original_input << "\n";
		file1_writer << "Output:\n" << (data.eol.empty() ? output1 : ojtt::string::replaceAll(output1, "\n", data.eol + "\n")) << "\n";
		file2_writer << "Input:\n" << original_input << "\n";
		file2_writer << "Output:\n" << (data.eol.empty() ? output2 : ojtt::string::replaceAll(output2, "\n", data.eol + "\n")) << "\n";
	}
	file1_writer.close();
	file2_writer.close();
	if (data.diff_level == 2) {
		// Show in diff GUI.
		std::string _2;
		if (ret = ot::execute(data.diff, "", _2, file1, file2, "", data.time_out, _, std::cout)) return 1;
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
		if (ret = ot::compile(data.compile, data.file, data.tmp_dir_uuid, data.output_file, std::cout)) return exit(ret, data.pause);
		if (data.test_single) {
			std::cout << "\n";
			int ac = 0;
			if (test_single(ac, data))
				return exit(ret, data.pause);
			// Report result.
			if (ac == data.input_output.size())
				std::cout << ac << "/" << data.input_output.size() << " AC (All Accepted)\n";
			else
				std::cout << ac << "/" << data.input_output.size() << " NA (Not Accepted)\n";
		} else {
			// Compile diff file.
			if (ret = ot::compile(data.compile, data.diff_file, data.tmp_dir_uuid, data.output_file, std::cout)) return exit(ret, data.pause);
			//Compile randomizer.
			if (ret = ot::compile(data.randomizer_compile, data.input_randomizer, data.tmp_dir_uuid, data.output_file, std::cout)) return exit(ret, data.pause);
			std::cout << "\n";
			if (test_double(data))
				return exit(ret, data.pause);
		}
		// End.
		return exit(0, data.pause);
	} catch (boost::program_options::error& e) {
		std::cout << "Unknown error occurred, please contact the developer for help.";
		std::cout << e.what();
		return exit(ret, data.pause);
	}
}
