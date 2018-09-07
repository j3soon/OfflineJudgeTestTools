#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "filesystem_home.hpp"
#include "config_data.hpp"
#define DEBUG_MODE

namespace ojtt {
	config_data::config_data(const std::string& version) {
		this->version = version;
	}

	//TODO: Split function to pieces by notifier?
	int config_data::setup_args(int ac, char *av[], std::ostream& cout) {
		using namespace std;
		namespace po = boost::program_options;
		namespace fs = boost::filesystem;
		namespace ofs = ojtt::filesystem;

		//TODO: Use 'this' instead of alias below.
		config_data& data = *this;

		// Descriptions.
		po::options_description desc(string("Usage: \n") +
			"1. ojtt [options] [file] [input1] [output1] [input2] [output2]...\n" +
			"2. ojtt [options] [file] -d [diff-file] -r [input-randomizer]");
		po::options_description generic("Generic options");
		po::options_description config("Configuration");
		generic.add_options()
			("help", "show help message")
			("version", "show executable version")
			("compile,c", po::value<vector<string>>()->composing(), "command to compile the source file")
			("config", po::value<string>(), "path of config file")
			("file,f", po::value<string>()->required(), "file to test")
			("file-input,I", po::value<string>(), "input file path that will be read by code file.")
			("file-output,O", po::value<string>(), "output file path that will be written by code file.")
			("input-output,i", po::value<vector<string>>()->composing(), "key-value pairs of input test files and the matching expected output files.")
			("diff-file,d", po::value<string>(), "file to diff")
			("input-randomizer,r", po::value<string>(), "file to generate random inputs")
			;
		config.add_options()
			("tmp-dir", po::value<string>()->default_value((fs::temp_directory_path() / "ojtt").string()), "directory for saving temp files")
			("eol", po::value<string>()->default_value("<EOL>"), "end-of-line symbol will show up")
			("universal-eol", po::value<bool>()->default_value(true), "ignore end-of-line differences")
			("diff", po::value<string>(), "diff UI's command")
			;
		desc.add(generic).add(config);
		//The code above looks like Lisp code haha.
		po::positional_options_description p;
		p.add("file", 1).add("input-output", -1);

		// Validate args.
		po::variables_map vm;
		po::store(po::command_line_parser(ac, av).
			options(desc).positional(p).run(), vm);

		if (vm.count("help")) {
			cout << desc << "\n";
			return 1;
		}
		if (vm.count("version")) {
			cout << "ojtt version " << data.version << "\n";
			return 1;
		}
		if (vm.count("file")) {
			string ss = vm["file"].as<string>();
			data.file = fs::weakly_canonical(vm["file"].as<string>()).string();
			data.file_ext = fs::extension(data.file);
			if (data.file_ext.length() > 0)
				data.file_ext = data.file_ext.substr(1);
		}
		else {
			cout << "File was not set.\n" << desc << "\n";
			return 1;
		}
		if (vm.count("config")) {
			po::store(parse_config_file(vm["config"].as<string>().c_str(), desc), vm);
		}
		else {
			fs::path cfg = ofs::home_directory_path() / "ojtt" / ("ojtt-" + data.file_ext + ".cfg");
			if (!fs::exists(cfg))
				cfg = ofs::home_directory_path() / "ojtt" / "ojtt.cfg";
			if (fs::exists(cfg))
				po::store(parse_config_file(cfg.string().c_str(), desc), vm);
		}
		if (vm.count("compile")) {
			data.compile = vm["compile"].as<vector<string>>();
		}
		else {
			cout << "Compile command was not set.\n" << desc << "\n";
			return 1;
		}
		if (vm.count("tmp-dir")) {
			data.tmp_dir = vm["tmp-dir"].as<string>();
		}
		if (vm.count("eol")) {
			data.eol = vm["eol"].as<string>();
		}
		if (vm.count("universal-eol")) {
			data.universal_eol = vm["universal-eol"].as<bool>();
		}
		if (vm.count("diff")) {
			data.diff = vm["diff"].as<string>();
		}
		if (vm.count("diff-file")) {
			data.test_single = false;
			//For Usage 2.
			data.diff_file = vm["diff-file"].as<string>();
			if (vm.count("input-randomizer")) {
				data.input_randomizer = vm["input-randomizer"].as<string>();
			}
			else {
				cout << "Input randomizer was not set.\n" << desc << "\n";
				return 1;
			}
		}
		else if (vm.count("input-output")) {
			data.test_single = true;
			// For Usage 1.
			vector<string> io = vm["input-output"].as<vector<string>>();
			if (io.size() % 2 != 0) {
				cout << "Inputs outputs were not in pair.\n" << desc << "\n";
				return 1;
			}
			string s;
			int i = 0;
			for (auto it : io) {
				i++;
				if (i == 2) {
					i = 0;
					data.input_output.push_back(make_pair(s, it));
				}
				s = it;
			}
		}
		else {
			cout << "You should set at least one of input-output and diff-file.\n" << desc << "\n";
			return 1;
		}
		if (vm.count("file-input")) {
			data.file_input = vm["file-input"].as<string>();
		}
		if (vm.count("file-output")) {
			data.file_output = vm["file-output"].as<string>();
		}

		po::notify(vm);
#ifdef DEBUG_MODE
		cout << data.str();
#endif
		return 0;
	}
}
