#ifndef OJTT_LAUNCHER_H
#define OJTT_LAUNCHER_H
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <string>
#include <sstream>

namespace ojtt {
	//TODO: Encapsulate.
	struct launcher_result {
		enum Result {
			RESULT_UNSET,
			RESULT_EXCEPTION,
			RESULT_ERROR,
			RESULT_TIMEOUT,
			RESULT_SUCCESS,
		};
		Result result = RESULT_UNSET;
		std::string ex_what;
		int exit_code;
		long long exec_time;
		std::string output;

		//TODO: Use more straightforward approach.
		void setResult(Result result) {
			if (this->result == RESULT_UNSET)
				this->result = result;
		}
	};
	struct launcher {
		static inline launcher_result launch(const std::string& exec, int time_out = -1, std::string input = "", int buff_size = 4096) {
			launcher_result ret;
			namespace bp = boost::process;
			boost::asio::io_service ios;
			std::vector<char> buf(buff_size);
			std::vector<char> inputbuff = std::vector<char>(input.begin(), input.end());
			try {
				//TODO: Set up group to terminate child child process.
				//However there will be error when executing the line below.
				/*bp::group g;
				bp::child c(exec, g, bp::std_in < boost::asio::buffer(inputbuff), (bp::std_out & bp::std_err) > boost::asio::buffer(buf), ios);
				ios.run();
				if (!g.wait_for(std::chrono::milliseconds(time_out))) {
					g.terminate();
					ret.setResult(launcher_result::RESULT_TIMEOUT);
				}*/
				bp::child c(exec, bp::std_in < boost::asio::buffer(inputbuff), (bp::std_out & bp::std_err) > boost::asio::buffer(buf), ios);
				std::chrono::time_point<std::chrono::system_clock> start_clk = std::chrono::system_clock::now();
				if (time_out == -1) {
					ios.run();
				} else {
					int a = ios.run_for(std::chrono::milliseconds(time_out));
					//TODO: Change to execute terminate in certain condition.
					c.terminate();
				}
				std::chrono::time_point<std::chrono::system_clock> end_clk = std::chrono::system_clock::now();
				auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_clk - start_clk);
				auto ms = milliseconds.count();
				ret.exec_time = ms;
				/*if (time_out != -1 && !c.wait_for(std::chrono::milliseconds(0))) {
					c.terminate();
					ret.setResult(launcher_result::RESULT_TIMEOUT);
				}*/
				//Wait for the exit code.
				c.wait();
				ret.exit_code = c.exit_code();
				if (ret.exit_code) {
					ret.setResult(launcher_result::RESULT_TIMEOUT);
				}
			} catch (std::system_error& e) {
				ret.setResult(launcher_result::RESULT_EXCEPTION);
				ret.ex_what = e.what();
				return ret;
			}
			std::stringstream ss;
			for (auto it : buf) {
				if (it == 0) break;
				ss << it;
			}
			ret.output = ss.str();
			if (ret.exit_code != 0)
				ret.setResult(launcher_result::RESULT_ERROR);
			ret.setResult(launcher_result::RESULT_SUCCESS);
			return ret;
		}
	};
}
#endif