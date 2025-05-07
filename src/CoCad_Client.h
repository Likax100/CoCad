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

    void RequestBecomeSH(std::string name) {
      CoCadNet::msg<MessageTypes> m;
      m.head.ID = MessageTypes::ccRequestSessionHost;
      m.dat = name;
      m.head.size = m.dat.size();
      send_msg(m);
    }

    void RequestJoinSH(std::string key) {
      CoCadNet::msg<MessageTypes> m;
      m.head.ID = MessageTypes::ccRequestJoinSession;
      m.dat = key;
      m.head.size = m.dat.size();
      send_msg(m);
    }

    void RequestEditorRepr() {
      CoCadNet::msg<MessageTypes> m;
      m.head.ID = MessageTypes::ccOpRequestSHModelData;
      send_msg(m);
    }
};

