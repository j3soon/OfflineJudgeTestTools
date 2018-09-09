#ifndef OJTT_LAUNCHER
#define OJTT_LAUNCHER
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
		std::string output;

		//TODO: Use more straightforward approach.
		void setResult(Result result) {
			if (this->result == RESULT_UNSET)
				this->result = result;
		}
	};
	struct launcher {
		static launcher_result launch(const std::string& exec, int time_out = -1, std::string input = "", int buff_size = 4096) {
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
				ios.run();
				if (time_out != -1 && !c.wait_for(std::chrono::milliseconds(time_out))) {
					c.terminate();
					ret.setResult(launcher_result::RESULT_TIMEOUT);
				}
				//Wait for the exit code.
				c.wait();
				ret.exit_code = c.exit_code();
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