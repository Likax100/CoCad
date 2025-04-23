#pragma once

#include "CoCad_Common.h"
#include "CoCad_SafeQueue.h"
#include "CoCad_Message.h"
#include "CoCad_Connection.h"

namespace CoCadNet
{
	template<typename T>
	class i_Server {
		public:
			i_Server(unsigned int port) : si_asio_acceptor(si_asio_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) { }

			virtual ~i_Server() { end_server(); }

			bool start_server() {
				try {
					wait_for_connections(); // esure asio has work so it doesnt just quit immediately
					si_thread_context = std::thread([this]() { si_asio_context.run(); });
				} catch (std::exception& e) {
					std::cerr << "ERROR-[SERVER] Could not start, details: " << e.what() << "\n";
					return false;
				}

				std::cout << "STATUS-[SERVER] Started Server..\n";
				return true;
			}

			void end_server() {
				si_asio_context.stop();
				if (si_thread_context.joinable()) si_thread_context.join();
				std::cout << "STATUS-[SERVER] Server Closed..\n";
			}

			void wait_for_connections() {
				// important so that asio keeps listening to incoming connections
				// note: acceptor object does this, opens new sockets for said connections
				si_asio_acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
					if (!ec) {
						std::cout << "STATUS-[SERVER] New Connection established @ " << socket.remote_endpoint() << "\n";
						// connection object to handle new client
						std::shared_ptr<CoCadNet::Connection<T>> new_connection = std::make_shared<CoCadNet::Connection<T>>(CoCadNet::Connection<T>::Ownership::ServerOwned, 
								si_asio_context, std::move(socket), si_msgs_in);
						
						if (on_client_connect(new_connection)) { // this fails if user denies connection						
							si_valid_connections.push_back(std::move(new_connection));
							si_valid_connections.back()->connect_to_client(si_ID_Counter++); // IMPORTANT, prime clients asio context to listen to incoming messages
							std::cout << "STATUS-[SERVER:" << si_valid_connections.back()->get_id() << "] Connection Approved\n";
						} else { std::cout << "STATUS-[SERVER:--] Connection Denied\n"; }
					} else { std::cout << "ERROR-[SERVER] Could Not Accept New Connection, details: " << ec.message() << "\n"; }

					wait_for_connections(); // wait for more connections
				});
			}

			// ============== // SERVER UTIL FUNCS // ================ //
			void send_msg_to_client(std::shared_ptr<CoCadNet::Connection<T>> client, const CoCadNet::msg<T>& m) {
				if (client && client->is_connected()) { client->send_msg(m); }
				else { // remove client if we cannot contact it - safet reasons + memory
					on_client_disconnect(client);
					client.reset();
					si_valid_connections.erase(std::remove(si_valid_connections.begin(), si_valid_connections.end(), client), si_valid_connections.end());
				}
			}
			
			void broadcast_msg(const CoCadNet::msg<T>& m, std::shared_ptr<CoCadNet::Connection<T>> ignored_client = nullptr) {
				bool invalid_client_exists_in_queue = false;

				for (auto& client : si_valid_connections) {
					if (client && client->is_connected()) {
						if(client != ignored_client) { client->send_msg(m); }
					} else {
						// couldnt contact client so disconnect it like before
						on_client_disconnect(client);
						client.reset();
						invalid_client_exists_in_queue = true;
					}
				}

				// remove all disconnected clients
				if (invalid_client_exists_in_queue) { si_valid_connections.erase(std::remove(si_valid_connections.begin(), si_valid_connections.end(), nullptr), 
					si_valid_connections.end()); 
				}
			}

			// ensures that server is forced to respond to messages
			void update_server(int max_messages = -1, bool wait = false) {
				if (wait) si_msgs_in.wait();

				int processed_message_count = 0;
				while (processed_message_count < max_messages && !si_msgs_in.empty()) {
					auto msg = si_msgs_in.pop_front();
					on_message_received(msg.remote, msg.m);
					processed_message_count++;
				}
			}

		protected:
			// customisation of servers is in these classes as u are meant to override them,
			// + each are called when said event happens 
			virtual bool on_client_connect(std::shared_ptr<CoCadNet::Connection<T>> client) { return false; }
			virtual void on_client_disconnect(std::shared_ptr<CoCadNet::Connection<T>> client) { }
			virtual void on_message_received(std::shared_ptr<CoCadNet::Connection<T>> client, CoCadNet::msg<T>& m) { }

		protected:
			CoCadNet::SafeQueue<CoCadNet::signed_msg<T>> si_msgs_in;
			std::deque<std::shared_ptr<CoCadNet::Connection<T>>> si_valid_connections; // contains active valid connections
			asio::io_context si_asio_context;
			std::thread si_thread_context;
			asio::ip::tcp::acceptor si_asio_acceptor; // handles new incoming connection attempts

			unsigned int si_ID_Counter = 1;
	};
}
