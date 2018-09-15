#ifndef OJTT_CONFIG_DATA_H
#define OJTT_CONFIG_DATA_H

namespace ojtt {
	//TODO: Encapsulate.
	struct config_data {
		std::string compile, execute;
		std::string file;
		std::set<std::pair<std::string, std::string>> input_output;
		std::string file_input, file_output;
		std::string output_file;
		std::string diff_file;
		std::string input_randomizer;
		std::string tmp_dir;
		std::string tmp_dir_uuid;
		std::string eol;
		bool universal_eol;
		std::string randomizer_compile;
		std::string diff;
		int time_out;
		int diff_level;
		bool pause = false;
		bool test_single;
		std::string version;

		explicit config_data(std::string version);

		/// <summary>Set up help description.</summary>
		/// <param name="ac">Arguments count.</param>
		/// <param name="av">Array of arguments sent through call, executable filename is also in it.</param>
		/// <param name="cout">Output messages whereabouts.</param>
		/// <returns>Returns 0 if all args seems valid, otherwise 1.</returns>
		int setup_args(int ac, char *av[], std::ostream& cout);

		/// <summary>For debugging purpose.</summary>
		/// <returns>Returns config data in std::string format.</returns>
		std::string str() {
			std::string s;
			s += "compile: \n";
			for (auto it : compile)
				s += it + "\n";
			s += "file: \n" + file + "\n";
			s += "input_output: \n";
			for (auto it : input_output)
				s += it.first + ", " + it.second + "\n";
			s += "file_input: \n" + file_input + "\n";
			s += "file_output: \n" + file_output + "\n";
			s += "diff_file: \n" + diff_file + "\n";
			s += "input_randomizer: \n" + input_randomizer + "\n";
			s += "tmp_dir: \n" + tmp_dir + "\n";
			s += "eol: \n" + eol + "\n";
			s += "universal_eol: \n" + std::to_string(universal_eol) + "\n";
			s += "diff: \n" + diff + "\n";
			s += "time_out: \n" + std::to_string(time_out) + "\n";
			s += "pause: \n" + std::to_string(pause) + "\n";
			s += "test_single: \n" + std::to_string(test_single) + "\n";
			return s;
		}
	};
}
#endif