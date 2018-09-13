#ifndef OJTT_TESTING_H
#define OJTT_TESTING_H
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include "launcher.hpp"

namespace ojtt {
	namespace testing {
		//TODO: Optimize by storing replaced string for further use.
		// Preprocess command.
		inline std::string _preprocess_path(const std::string& file, const std::string& command, const std::string& tempdir) {
			namespace fs = boost::filesystem;
			std::string cmd = command;
			std::string file_ext = fs::extension(file);
			if (file_ext.length() > 0)
				file_ext = file_ext.substr(1);
			std::string file_name = fs::path(file).stem().string();
			std::string	file_dir = fs::path(file).parent_path().string();
			//Replace <filepath> == <filedirpath><filename><fileext>
			cmd = string::replaceAll(cmd, "<filepath>", "\"" + file + "\"");
			cmd = string::replaceAll(cmd, "<filedirpath>", file_dir + "/");
			cmd = string::replaceAll(cmd, "<filename>", file_name);
			cmd = string::replaceAll(cmd, "<fileext>", file_ext);
			cmd = string::replaceAll(cmd, "<tmpdir>", tempdir);
			return cmd;
		}
		inline std::string _preprocess_diff_path(const std::string& expected_file, const std::string& actual_file, const std::string& command) {
			std::string cmd = command;
			cmd = string::replaceAll(cmd, "<expected_file>", expected_file);
			cmd = string::replaceAll(cmd, "<actual_file>", actual_file);
			return cmd;
		}
		inline std::string universal_eol(std::string str, bool universal_eol) {
			if (!universal_eol)
				return str;
			str = string::replaceAll(str, "\r\n", "\n");
			str = string::replaceAll(str, "\r", "\n");
			return str;
		}
		inline int compile(const std::string& command, const std::string& file, const std::string& tempdir, std::ostream& cout) {
			namespace bp = boost::process;
			// Preprocess command.
			std::string cmd = _preprocess_path(file, command, tempdir);
			// Execute compile command.
			cout << "Executing command: " << cmd << "\n";
			launcher_result lresult;
			lresult = launcher::launch(cmd);
			if (lresult.result == launcher_result::RESULT_EXCEPTION) {
				cout << lresult.ex_what << "\n";
				return 1;
			}
			cout << "Output: \n" << lresult.output << "\n";
			cout << "Exit with code: " << lresult.exit_code << "\n";
			if (lresult.result != launcher_result::RESULT_SUCCESS)
				return 1;
			return 0;
		}
		inline int execute(const std::string& command, const std::string& input, std::string& output, const std::string& file, const std::string& tempdir, int time_out, long long& exec_time, std::ostream& cout, boost::filesystem::ofstream* pActual_writer = nullptr) {
			namespace bp = boost::process;
			// Preprocess command.
			std::string cmd = _preprocess_path(file, command, tempdir);
			//TODO: Change below dirty code (1 line) to something readable.
			// Code below pretend 'file' is 'expected_file', 'tempdir' is 'actual_file'.
			cmd = _preprocess_diff_path(file, tempdir, command);
			// Execute executable.
			launcher_result lresult;
			lresult = launcher::launch(cmd, time_out, input);
			exec_time = lresult.exec_time;
			if (lresult.result == launcher_result::RESULT_EXCEPTION) {
				cout << "Error in function 'execute' when executing command: " << cmd << "\n";
				cout << lresult.ex_what << "\n";
				if (pActual_writer == nullptr) return 1;
				(*pActual_writer) << "Error in function 'execute' when executing command: " << cmd << "\n";
				(*pActual_writer) << lresult.ex_what << "\n";
				return 1;
			}
			output = lresult.output;
			if (lresult.result == launcher_result::RESULT_ERROR) {
				cout << "RE (Runtime Error)\n";
				cout << "Output: \n" << lresult.output << "\n";
				cout << "Exit with code: " << lresult.exit_code << "\n";
				if (pActual_writer == nullptr) return 1;
				(*pActual_writer) << "RE (Runtime Error)\n";
				(*pActual_writer) << "Output: \n" << lresult.output << "\n";
				(*pActual_writer) << "Exit with code: " << lresult.exit_code << "\n";
				return 1;
			}
			if (lresult.result == launcher_result::RESULT_TIMEOUT) {
				cout << "TLE (Time Limit Exceeded)\n";
				cout << "Output: \n" << lresult.output << "\n";
				cout << "Exit with code: " << lresult.exit_code << "\n";
				if (pActual_writer == nullptr) return 1;
				(*pActual_writer) << "TLE (Time Limit Exceeded)\n";
				(*pActual_writer) << "Output: \n" << lresult.output << "\n";
				(*pActual_writer) << "Exit with code: " << lresult.exit_code << "\n";
				return 1;
			}
			return 0;
		}
		inline int read(const std::string& path, std::string& content, const std::string& file, const std::string& tempdir, std::ostream& cout) {
			namespace fs = boost::filesystem;
			// Preprocess path.
			std::string p = _preprocess_path(file, path, tempdir);
			// Read from file.
			try {
				// Read entire file.
				fs::ifstream reader(p);
				//TODO: Change to 'Rvalue References' instead? e.g. std::string&& str...
				// parenthesis below is required to prevent The Most Vexing Parse problem.
				std::string str((std::istreambuf_iterator<char>(reader)),
					std::istreambuf_iterator<char>());
				content = str;
				reader.close();

			} catch (fs::filesystem_error& e) {
				cout << "Error in function 'read' when reading from file: " << p << "\n";
				cout << e.what() << "\n";
				return 1;
			}
			return 0;
		}
		inline int overwrite(const std::string& path, const std::string& input, const std::string& file, const std::string& tempdir, std::ostream& cout) {
			namespace fs = boost::filesystem;
			// Preprocess path.
			std::string p = _preprocess_path(file, path, tempdir);
			// Write to file.
			try {
				fs::ofstream writer(p);
				writer << input;
				writer.close();
			} catch (fs::filesystem_error& e) {
				cout << "Error in function 'overwrite' when writing to file: " << p << "\n";
				cout << e.what() << "\n";
				return 1;
			}
			return 0;
		}
	};
}
#endif