#pragma once
#include <iostream>
#include "CoCad_Networking.h"
#include "CoCad_ConnectionMessageTypes.h"

class CoCadClient : public CoCadNet::i_Client<MessageTypes> {
	public:
		void PingServer() {
			CoCadNet::msg<MessageTypes> m;
			m.head.ID = MessageTypes::ServerPing;	
      send_msg(m);
		}

    void AuthenticateLogin(std::string usr, std::string pass) {
      std::cout << "TESTING TESTING " << usr << " " << pass << "\n";
      CoCadNet::msg<MessageTypes> m;
      m.head.ID = MessageTypes::ccRequestAuthentication;
      m.dat.push_back(usr);
      m.dat.push_back(pass);
      m.head.size = m.dat.size();
      send_msg(m);
    }
};

/*
int main()
{
	CustomClient c;
	c.connect("127.0.0.1", 60000);


	if (c.is_connected()) {
		if (!c.get_incoming_queue().empty()) {
			auto m = c.get_incoming_queue().pop_front().m;

			switch (m.head.ID) {
				case CustomMsgTypes::ServerAccept: {			
					std::cout << "STATUS-[CLIENT] Server Accepted Connection\n";
				} break;

				case CustomMsgTypes::ServerPing:{
					std::cout << "STATUS-[CLIENT] Ping Returned By Server\n";
				} break;
			}
		}
	}

	return 0;
}
*/
