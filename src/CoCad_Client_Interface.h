#pragma once
#include "CoCad_Common.h"

namespace CoCadNet
{
	template <typename T>
	class i_Client {
	public:
		i_Client() { }
		virtual ~i_Client() { disconnect(); }

		bool connect(const std::string& host_name, const unsigned int port) {
			try {
				// use the asio resolver to resolve host_name -> actual ip
				asio::ip::tcp::resolver resolver(ci_context);
				asio::ip::tcp::resolver::results_type end_points = resolver.resolve(host_name, std::to_string(port));

				// creating connection
				ci_connection = std::make_unique<CoCadNet::Connection<T>>(CoCadNet::Connection<T>::Ownership::ClientOwned, ci_context, asio::ip::tcp::socket(ci_context), ci_msgs_in);
				ci_connection->connect_to_server(end_points);
				ci_thread_context = std::thread([this]() { ci_context.run(); });
			} catch (std::exception& e) {
				std::cerr << "ERROR-[CLIENT] Could Not Connect To Server, details: " << e.what() << "\n";
				return false;
			}

			std::cout << "STATUS-[CLIENT] Successfully Connected To Server..\n"; 
			return true;
		}

		bool is_connected() {
			if (ci_connection) { return ci_connection->is_connected(); }
			else { return false; }
		}

		void disconnect() {
			if(is_connected()) { ci_connection->disconnect(); }
			// cleanup 			
			ci_context.stop();
			if (ci_thread_context.joinable()) { ci_thread_context.join(); }
			ci_connection.release();
		}

		void send_msg(const CoCadNet::msg<T>& m) {
			if (is_connected()) { 
        ci_connection->send_msg(m); 
      }
		}

		// getter for queue of messages from server, for processing on client end
		CoCadNet::SafeQueue<CoCadNet::signed_msg<T>>& get_incoming_queue() { return ci_msgs_in; }

	protected:
		asio::io_context ci_context;
		std::thread ci_thread_context;
		std::unique_ptr<CoCadNet::Connection<T>> ci_connection;
		
	private:
		CoCadNet::SafeQueue<CoCadNet::signed_msg<T>> ci_msgs_in; // from server
	};
}
