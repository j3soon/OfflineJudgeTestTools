#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include "config_data.hpp"
#include "filesystem_home.hpp"
#define DEBUG_MODE

/// <summary>Set up help description.</summary>
/// <param name="ac">Arguments count.</param>
/// <param name="av">Array of arguments sent through call, executable filename is also in it.</param>
/// <returns>Returns 0 if all args seems valid, otherwise 1.</returns>
int setup_args(int ac, char *av[]);

static const std::string version = "alpha";

/// <summary>Should be called directly by system.</summary>
/// <param name="argc">Arguments count.</param>
/// <param name="argv">Array of arguments sent through call, executable filename is also in it.</param>
int main(int argc, char *argv[]) {
	try {
		int ret = setup_args(argc, argv);
		if (ret != 0)
			return ret;
	} catch (boost::program_options::error& e) {
		std::cout << e.what();
		return 1;
	}
}

int setup_args(int ac, char *av[]) {
	using namespace std;
	namespace po = boost::program_options;
	namespace fs = boost::filesystem;
	namespace ofs = ojtt::filesystem;

	// Options.
	ojtt::config_data data;

	// Descriptions.
	po::options_description desc("Usage: ojtt [options] [file] [input1] [output1] [input2] [output2]...");
	po::options_description generic("Generic options");
	po::options_description config("Configuration");
	generic.add_options()
		("help", "show help message")
		("version", "show executable version")
		("compile,c", po::value<vector<string>>()->composing(), "command to compile the source file")
		("config", po::value<string>(), "path of config file")
		("file,f", po::value<string>()->required(), "file to test")
		("input-output,i", po::value<vector<string>>()->composing(), "key-value pairs of input files and the matching expected output files.")
		;
	config.add_options()
		("tmp-dir", po::value<string>()->default_value((fs::temp_directory_path() / "ojtt").string()), "directory for saving temp files")
		("eol", po::value<string>()->default_value("<EOL>"), "end-of-line symbol will show up")
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
		cout << "ojtt version " << version << "\n";
		return 1;
	}
	if (vm.count("file")) {
		string ss = vm["file"].as<string>();
		data.file = fs::weakly_canonical(vm["file"].as<string>()).string();
		data.file_ext = fs::extension(data.file);
		if (data.file_ext.length() > 0)
			data.file_ext = data.file_ext.substr(1);
	} else {
		cout << "File was not set.\n" << desc << "\n";
		return 1;
	}
	if (vm.count("config")) {
		po::store(parse_config_file(vm["config"].as<string>().c_str(), desc), vm);
	} else {
		fs::path cfg = ofs::home_directory_path() / "ojtt" / ("ojtt-" + data.file_ext + ".cfg");
		if (!fs::exists(cfg))
			cfg = ofs::home_directory_path() / "ojtt" / "ojtt.cfg";
		if (fs::exists(cfg))
			po::store(parse_config_file(cfg.string().c_str(), desc), vm);
	}
	if (vm.count("compile")) {
		data.compile = vm["compile"].as<vector<string>>();
	} else {
		cout << "Compile command was not set.\n" << desc << "\n";
		return 1;
	}
	if (vm.count("input-output")) {
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
	} else {
		cout << "Inputs outputs were not set.\n" << desc << "\n";
		return 1;
	}
	if (vm.count("tmp-dir")) {
		data.tmp_dir = vm["tmp-dir"].as<string>();
	}
	if (vm.count("eol")) {
		data.eol = vm["eol"].as<string>();
	}

	po::notify(vm);
#ifdef DEBUG_MODE
	cout << data.str();
#endif

	return 0;
}