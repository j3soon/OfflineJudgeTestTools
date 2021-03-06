#include "ojtt.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/regex.hpp>
#include <vector>
#include <map>
#include "config_data.hpp"
#include "testing.hpp"

//#define DEBUG_MODE

namespace ojtt {
	config_data::config_data(std::string version) : version(std::move(version)) {}

	//TODO: Split function to pieces by notifier?
	int config_data::setup_args(int ac, char *av[], std::ostream& cout) {
		namespace po = boost::program_options;
		namespace fs = boost::filesystem;
		namespace ofs = filesystem;
		namespace os = string;

		//TODO: Use 'this' instead of alias below.
		config_data& data = *this;

		// Descriptions.
		po::options_description desc(std::string("Usage: \n") +
			"1. ojtt [options] [file] ([input1] [output1] [input2] [output2]...)\n" +
			"2. ojtt [options] [file] -d [diff-file] -r [input-randomizer]\n" +
		    "3. ojtt [options] [file] -c ([input1] [output1] [input2] [output2]...)");
		po::options_description generic("Generic options");
		po::options_description config("Configuration");
		generic.add_options()
			("help,?", "show help message")
			("version,v", "show executable version")
			("clean,cln", "clean temp files")
			("config", po::value<std::string>(), "path of config file")
			("file,f", po::value<std::string>()/*->required()*/, "file to test")
			("file-input,i", po::value<std::string>(), "input file path that will be read by code file")
			("file-output,o", po::value<std::string>(), "output file path that will be written by code file")
			("input-output,t", po::value<std::vector<std::string>>()->composing(), "key-value pairs of input test files and the matching expected output files")
			("diff-file,d", po::value<std::string>(), "the 2nd file to test (diff)")
			("input-randomizer,r", po::value<std::string>(), "file to generate random inputs")
			("create,c", po::bool_switch()->default_value(false), "create output file for corresponding inputs if not exist")
			("test-folder,t", po::value<std::vector<std::string>>()->composing(), "folders that might contain test cases")
			;
		config.add_options()
			("tmp-dir", po::value<std::string>()->default_value((fs::temp_directory_path() / "ojtt").string()), "directory for saving temp files")
			("eol", po::value<std::string>()->default_value("<EOL>"), "end-of-line symbol will show up")
			("universal-eol,ueol", po::value<bool>()->default_value(true), "ignore end-of-line differences")
			("compile,C", po::value<std::string>(), "command to compile the source file")
			("execute,E", po::value<std::string>(), "command to execute the output file")
			("output-file,O", po::value<std::string>(), "output file")
			("file-random,R", po::value<std::string>(), "input random file path that was produced by the randomizer. (use this to avoid buffer overflow)")
			("randomizer-compile", po::value<std::string>(), "command to compile the randomizer file (if not set uses 'compile' instead)")
			("diff,D", po::value<std::string>(), "diff UI's command")
			("time-out,tle,T", po::value<int>()->default_value(15000), "executable max running time in milliseconds")
			("time-log,tlog", po::value<int>()->default_value(100), "how many times for randomizer to match before print.")
			("diff-level,L", po::value<int>()->default_value(1), "0 for don't show difference\n1 for show all directly\n2 for show by 'diff' command")
			("pause,P", po::value<bool>()->default_value(false), "pause when exit")
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
		if (vm.count("tmp-dir")) {
			data.tmp_dir = vm["tmp-dir"].as<std::string>();
			boost::uuids::uuid uuid = boost::uuids::random_generator()();
			data.tmp_dir_uuid = (fs::path(data.tmp_dir) / boost::uuids::to_string(uuid)).string();
			try {
				fs::create_directories(data.tmp_dir_uuid);
			} catch (fs::filesystem_error& e) {
				cout << "Error in function 'setup_args' at arg 'tmp-dir' when trying to create: " << data.tmp_dir_uuid << "\n";
				cout << e.what();
				return 1;
			}
		}
		if (vm.count("clean")) {
			int num;
			try {
				num = fs::remove_all(vm["tmp-dir"].as<std::string>());
			} catch (fs::filesystem_error& e) {
				cout << "Error in function 'setup_args' at arg 'clean' when trying to remove: " << data.tmp_dir << "\n";
				cout << e.what();
				return 1;
			}
			cout << num << " temp files deleted\n";
			return 0;
		}
		if (vm.count("file")) {
			try {
				data.file = fs::canonical(vm["file"].as<std::string>()).string();
			} catch (fs::filesystem_error& e) {
				cout << "Error in function 'setup_args' at arg 'file' when trying to get: " << vm["file"].as<std::string>() << "\n";
				cout << e.what() << "\n";
				return 1;
			}
		} else {
			cout << "Warning: File was not set.\n" << desc << "\n";
		}
		if (vm.count("config")) {
			po::store(parse_config_file(vm["config"].as<std::string>().c_str(), desc), vm);
		}
		fs::path cfg = "ojtt.cfg";
		if (fs::exists(cfg))
			po::store(parse_config_file(cfg.string().c_str(), desc), vm);
		std::string file_ext = fs::extension(data.file);
		if (file_ext.length() > 0)
			file_ext = file_ext.substr(1);
		cfg = ofs::home_directory_path() / "ojtt" / ("ojtt-" + file_ext + ".cfg");
		if (fs::exists(cfg))
			po::store(parse_config_file(cfg.string().c_str(), desc), vm);
		cfg = ofs::home_directory_path() / "ojtt" / "ojtt.cfg";
		if (fs::exists(cfg))
			po::store(parse_config_file(cfg.string().c_str(), desc), vm);
		if (vm.count("compile")) {
			data.compile = vm["compile"].as<std::string>();
		} else {
			cout << "Compile command was not set.\n" << desc << "\n";
			return 1;
		}
		if (vm.count("execute")) {
			data.execute = vm["execute"].as<std::string>();
		} else {
			cout << "Execute command was not set.\n" << desc << "\n";
			return 1;
		}
		if (vm.count("output-file")) {
			data.output_file = vm["output-file"].as<std::string>();
		} else {
			cout << "Output file was not set.\n" << desc << "\n";
			return 1;
		}
		if (vm.count("eol")) {
			data.eol = vm["eol"].as<std::string>();
		}
		if (vm.count("universal-eol")) {
			data.universal_eol = vm["universal-eol"].as<bool>();
		}
		if (vm.count("randomizer-compile")) {
			data.randomizer_compile = vm["randomizer-compile"].as<std::string>();
		} else {
			data.randomizer_compile = data.compile;
		}
		if (vm.count("diff")) {
			data.diff = vm["diff"].as<std::string>();
		}
		if (vm.count("time-out")) {
			data.time_out = vm["time-out"].as<int>();
		}
		if (vm.count("time-log")) {
			data.time_log = vm["time-log"].as<int>();
		}
		if (vm.count("diff-level")) {
			data.diff_level = vm["diff-level"].as<int>();
			if (data.diff_level < 0 || data.diff_level > 2) {
				cout << "Diff level isn't recognized.\n" << desc << "\n";
				return 1;
			}
		}
		if (vm.count("pause")) {
			data.pause = vm["pause"].as<bool>();
		}
		if (vm.count("create")) {
			data.create = vm["create"].as<bool>();
		}
		if (vm.count("diff-file")) {
			data.test_single = false;
			//For Usage 2.
			try {
				data.diff_file = fs::canonical(vm["diff-file"].as<std::string>()).string();
			} catch (fs::filesystem_error& e) {
				cout << "Error in function 'setup_args' at arg 'diff-file' when trying to get: " << vm["diff-file"].as<std::string>() << "\n";
				cout << e.what() << "\n";
				return 1;
			}
			if (vm.count("input-randomizer")) {
				data.input_randomizer = vm["input-randomizer"].as<std::string>();
			} else {
				cout << "Input randomizer was not set.\n" << desc << "\n";
				return 1;
			}
		} else if (vm.count("input-output")) {
			if (vm.count("test-folder"))
				data.test_folder = vm["test-folder"].as<std::vector<std::string>>();
			data.test_folder.push_back("");
			data.test_single = true;
			// For Usage 1.
			std::vector<std::string> io = vm["input-output"].as<std::vector<std::string>>();
			if (io.size() % 2 != 0) {
				cout << "Inputs outputs were not in pair.\n" << desc << "\n";
				return 1;
			}
			std::string s1, s2;
			int i = 0;
			for (auto it : io) {
				i++;
				if (i == 2) {
					// Got a pair.
					std::map<std::string, std::string> input_map;
					std::map<std::string, std::string> output_map;
					i = 0;
					s2 = it;
					// Use RegEx to deal with '<any>'.
					std::string regex_s1 = "^" + os::replaceAll(ojtt::regex::escape(s1), "<any>", "(.*)") + "$";
					boost::regex reg1(regex_s1);
					std::string regex_s2 = "^" + os::replaceAll(ojtt::regex::escape(s2), "<any>", "(.*)") + "$";
					boost::regex reg2(regex_s2);
					boost::smatch what;

					try {
						for (auto str : data.test_folder) {
							for (auto f : fs::directory_iterator(
								(fs::path(data.file).parent_path() / str).string())) {
								// The 'file_name' below must be used indirectly or it'll have strange results.
								std::string file_name = f.path().filename().string();
								// TODO: (x) Deal with 'in.in' <any>.in in.<any>.
								if (boost::regex_match(file_name, what, reg1)) {
									input_map[what[1]] = str;
								}
								if (boost::regex_match(file_name, what, reg2)) {
									output_map[what[1]] = str;
								}
							}
						}
					} catch (fs::filesystem_error& e) {
						cout << "Error in function 'setup_args' at arg 'input-output' when trying to get parent of: " << data.file << "\n";
						cout << e.what() << "\n";
						return 1;
					}

					// Push matching set to vector.

					for (auto it : input_map) {
						if (output_map.find(it.first) != output_map.end()) {
							// TODO: Remove duplicated pair.
							// Found pair.
							data.input_output.insert(std::make_pair(
								(fs::path(it.second) / os::replaceAll(s1, "<any>", it.first)).string(),
								(fs::path(it.second) / os::replaceAll(s2, "<any>", it.first)).string()));
						} else {
							// Only input file found.
							data.input_only.insert(std::make_pair(
								(fs::path(it.second) / os::replaceAll(s1, "<any>", it.first)).string(),
								(fs::path(it.second) / os::replaceAll(s2, "<any>", it.first)).string()));
						}
					}
				}
				s1 = it;
			}
		} else {
			cout << "You should set at least one of input-output and diff-file.\n" << desc << "\n";
			return 1;
		}
		if (vm.count("file-input")) {
			data.file_input = vm["file-input"].as<std::string>();
			fs::path path(data.file_input);
			if (path.is_relative()) {
				std::string start_dir = testing::_preprocess_path(data.file, data.output_file, data.tmp_dir_uuid, data.output_file);
				path = fs::path(start_dir).parent_path() / data.file_input;
				file_input = path.string();
			}
		}
		if (vm.count("file-output")) {
			data.file_output = vm["file-output"].as<std::string>();
			fs::path path(data.file_output);
			if (path.is_relative()) {
				std::string start_dir = testing::_preprocess_path(data.file, data.output_file, data.tmp_dir_uuid, data.output_file);
				path = fs::path(start_dir).parent_path() / data.file_output;
				file_output = path.string();
			}
		}
		if (vm.count("file-random")) {
			data.file_random = vm["file-random"].as<std::string>();
			fs::path path(data.file_random);
			if (path.is_relative()) {
				std::string start_dir = testing::_preprocess_path(data.file, data.output_file, data.tmp_dir_uuid, data.output_file);
				path = fs::path(start_dir).parent_path() / data.file_random;
				file_random = path.string();
			}
		}

		po::notify(vm);
#ifdef DEBUG_MODE
		cout << data.str();
#endif
		return 0;
	}
}
