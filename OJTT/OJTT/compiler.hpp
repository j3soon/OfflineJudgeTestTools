#ifndef OJTT_COMPILER
#define OJTT_COMPILER
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <string>
#include "launcher.hpp"

namespace ojtt {
	struct compiler {
		// Preprocess command.
		static std::string _preprocess_path(const std::string& file, const std::string& command) {
			namespace fs = boost::filesystem;
			std::string cmd = command;
			std::string file_ext = fs::extension(file);
			if (file_ext.length() > 0)
				file_ext = file_ext.substr(1);
			std::string file_name = fs::path(file).stem().string();
			std::string	file_dir = fs::path(file).parent_path().string();
			//Replace <filepath> == <filedirpath><filename><fileext>
			cmd = string::replaceAll(cmd, "<filepath>", file);
			cmd = string::replaceAll(cmd, "<filedirpath>", file_dir);
			cmd = string::replaceAll(cmd, "<filename>", file_name);
			cmd = string::replaceAll(cmd, "<fileext>", file_ext);
			return cmd;
		}
		static int compile(const std::string& file, const std::string& command, std::ostream& cout) {
			namespace bp = boost::process;
			// Preprocess command.
			std::string cmd = _preprocess_path(file, command);
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
		static int execute(const std::string& file, const std::string& command, int time_out, const std::string& input, std::string output, std::ostream& cout) {
			namespace bp = boost::process;
			// Preprocess command.
			std::string cmd = _preprocess_path(file, command);
			// Execute executable.
			launcher_result lresult;
			lresult = launcher::launch(cmd, time_out, input);
			if (lresult.result == launcher_result::RESULT_EXCEPTION) {
				cout << "Executing command: " << cmd << "\n";
				cout << lresult.ex_what << "\n";
				return 1;
			}
			output = lresult.output;
			if (lresult.result != launcher_result::RESULT_SUCCESS) {
				cout << "Executing command: " << cmd << "\n";
				cout << "Output: \n" << lresult.output << "\n";
				cout << "Exit with code: " << lresult.exit_code << "\n";
				return 1;
			}
			return 0;
		}
	};
}
#endif