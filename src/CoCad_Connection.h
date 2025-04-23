#pragma once

#include "CoCad_Common.h"
#include "CoCad_SafeQueue.h"
#include "CoCad_Message.h"

namespace CoCadNet
{
	template<typename T>
	class Connection : public std::enable_shared_from_this<Connection<T>> {
	public:
		// A connection object is owned by a server or client (acting as an intermediatery for both hence the mixed functions)
		// this enum class allows us change behaviour depending on the ownership
		enum class Ownership { ServerOwned, ClientOwned };

		// param0 = owner of this connection object, param1 = asio context reference
		// param2 = socket for connection, param3 = reference to incoming message queue
		Connection(Ownership connection_owner, asio::io_context& asio_context, asio::ip::tcp::socket sock, CoCadNet::SafeQueue<CoCadNet::signed_msg<T>>& incoming_msg_queue)
		: c_glb_asio_context(asio_context), c_socket(std::move(sock)), c_msgs_in(incoming_msg_queue) {
			c_owned_by = connection_owner;
		}

		virtual ~Connection() { } 

		unsigned int get_id() const { return ID; }

		bool is_connected() const { return c_socket.is_open(); }

		void disconnect() {
			if (is_connected()) { asio::post(c_glb_asio_context, [this]() { c_socket.close(); }); }
		}

		// Only called by server
		void connect_to_client(unsigned int user_ID = 0) {
			if (c_owned_by == Ownership::ServerOwned) {
				if (c_socket.is_open()) { ID = user_ID; process_header(); }
			}
		}

		// Only called by client
		void connect_to_server(const asio::ip::tcp::resolver::results_type& end_points) {
			if (c_owned_by == Ownership::ClientOwned) {
				// tell asio to connect to end point
				asio::async_connect(c_socket, end_points, 
					[this](std::error_code ec, asio::ip::tcp::endpoint ep) { if (!ec) { process_header(); }
				});
			}
		}

		// note: connections are always 1-1 hence no need to specify the target address (client is assumed to be server etc)
		void send_msg(const CoCadNet::msg<T>& m) {
			asio::post(c_glb_asio_context, [this, m]() {
				// note: we must check if a message is currently being processed first
				// before calling priming functions, as this could otherwise affect the queue state mid processing (nature of async)
				bool currently_writting_message = !c_msgs_out.empty();
				c_msgs_out.push_back(m);
				if (!currently_writting_message) { 
          prime_header(); 
        }
			});
		}

	private:
		// main important async methods
		void prime_header() { // tell context to write header
			asio::async_write(c_socket, asio::buffer(&c_msgs_out.front().head, sizeof(CoCadNet::msg_head<T>)),
				[this](std::error_code ec, std::size_t length) {
					if (!ec) {
						// written and sent message - success! check if it also has some data
						if (c_msgs_out.front().dat.size() > 0) { prime_body(); }
						else {
							c_msgs_out.pop_front(); // remove it cuz we finished processing
							if (!c_msgs_out.empty()) { prime_header(); } // if queue not empty we have more processing to do
						}
					} else { std::cout << "ERROR-[CONNECTION:" << ID << "] Could not write header.\n"; c_socket.close(); }
			});
		}

		void prime_body() {
			asio::async_write(c_socket, asio::buffer(c_msgs_out.front().dat.data(), c_msgs_out.front().dat.size()),
				[this](std::error_code ec, std::size_t length) {
					if (!ec) {
						// if able to write data, full packet done and successful - remove from queue
						c_msgs_out.pop_front();
            std::cout << "Written " << length << " Bytes to buffer\n";
						if (!c_msgs_out.empty()) { prime_header(); } // give asio more work if not empty
					} else { std::cout << "ERROR-[CONNECTION:" << ID << "] Could not write body.\n"; c_socket.close(); }
				});
		}

		void process_header() {
			asio::async_read(c_socket, asio::buffer(&c_temp_msg_received.head, sizeof(CoCadNet::msg_head<T>)),
				[this](std::error_code ec, std::size_t length) {						
					if (!ec) {
						if (c_temp_msg_received.head.size > 0) {
							c_temp_msg_received.dat.resize(c_temp_msg_received.head.size);
							process_body();
						} else { add_to_incoming_messages(); } // packet had no data other than header so add to queue
					} else { std::cout << "ERROR-[CONNECTION:" << ID << "] Could not read header of message packet.\n"; c_socket.close(); }
			});
		}

		void process_body() {
			asio::async_read(c_socket, asio::buffer(c_temp_msg_received.dat.data(), c_temp_msg_received.dat.size()),
				[this](std::error_code ec, std::size_t length) {						
					if (!ec) { add_to_incoming_messages(); } // packet had data and data packets arrived so add to incoming messages
					else { std::cout << "ERROR-[CONNECTION:" << ID << "] Could not read header of message packet.\n"; c_socket.close(); }
			});
		}

		void add_to_incoming_messages() { // we	only get here once message fully received ready for handling		
			// note: convert it to a signed message to keep track of ownership
			
			//CoCadNet::signed_msg<T> sm;
			//sm.remote = this->shared_from_this();
			//sm.m = c_temp_msg_received;

			if(c_owned_by == Ownership::ServerOwned) { c_msgs_in.push_back({ this->shared_from_this(), c_temp_msg_received }); }
			else { c_msgs_in.push_back({ nullptr, c_temp_msg_received }); }

			process_header(); // call this to repeat message cycle - tell asio to wait for next message
		}

		protected: 
			// data is sent in async hence we need a var to hold and build the completed message from the "packets"
			CoCadNet::msg<T> c_temp_msg_received;
			Ownership c_owned_by = Ownership::ServerOwned;
			unsigned int ID = 0;

			asio::io_context& c_glb_asio_context; // note: this is the GLOBAL asio context, a ref
			asio::ip::tcp::socket c_socket; // connection objects connect to their owners hence require a socket

			CoCadNet::SafeQueue<CoCadNet::msg<T>> c_msgs_out; // messages to be sent out to address connected to this object (remote)
			CoCadNet::SafeQueue<CoCadNet::signed_msg<T>>& c_msgs_in; // reference to owners incoming queue
	};
}
