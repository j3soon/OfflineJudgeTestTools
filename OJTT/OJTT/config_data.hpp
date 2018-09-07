#ifndef OJTT_CONFIG_DATA
#define OJTT_CONFIG_DATA
#include <string>
#include <vector>

namespace ojtt {
	struct config_data {
		std::vector<std::string> compile;
		std::string file, file_ext;
		std::vector<std::pair<std::string, std::string>> input_output;
		std::string tmp_dir;
		std::string eol;

		/// <summary>For debugging purpose.</summary>
		/// <returns>Returns config data in std::string format.</returns>
		std::string str()
		{
			std::string s = "";
			s += "compile: \n";
			for (auto it : compile)
				s += it + "\n";
			s += "file: \n" + file + "\n";
			s += "file_ext: \n" + file_ext + "\n";
			s += "input_output: \n";
			for (auto it : input_output)
				s += it.first + ", " + it.second + "\n";
			s += "tmp_dir: \n" + tmp_dir + "\n";
			s += "eol: \n" + eol + "\n";
			return s;
		}
	};
}
#endif