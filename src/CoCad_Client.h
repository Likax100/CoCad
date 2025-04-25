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
      CoCadNet::msg<MessageTypes> m;
      m.head.ID = MessageTypes::ccRequestAuthentication;
      m.dat = usr + " " + pass;
      m.head.size = m.dat.size();
      send_msg(m);
    }

    void RequestBecomeSH() {
      CoCadNet::msg<MessageTypes> m;
      m.head.ID = MessageTypes::ccRequestSessionHost;
      send_msg(m);
    }

    void RequestJoinSH() {
      CoCadNet::msg<MessageTypes> m;
      m.head.ID = MessageTypes::ccRequestJoinSession;
      send_msg(m);
    }
};

