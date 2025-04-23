#include <iostream>
#include <map>
#include <string>

#include "CoCad_Networking.h"
#include "CoCad_ConnectionMessageTypes.h"
#include "CoCad_Utils.h"

class CoCadServer : public CoCadNet::i_Server<MessageTypes> {
	public:
		CoCadServer(unsigned int sel_port) : CoCadNet::i_Server<MessageTypes>(sel_port) { }

    void load_user_data(const char* acc_file_path) {
      if (FileUtil::FileExists(acc_file_path) && FileUtil::MatchesExt(acc_file_path, ".acc")) {
        auto content = FileUtil::ReadFileLines(acc_file_path);

        for (auto line : content) {
          auto split_line = StringUtil::SplitString(line, " ");
          if (split_line[0] == ".usr") { usr_acc_data[split_line[1]] = split_line[2]; }
        }
        
        is_usr_data_loaded = true;
        std::cout << "STATUS-[SERVER] User Data Loaded\n";
      } else { std::cout << "ERROR-[SERVER] Could Not Load Users (file must be of type .acc)\n";   }
    }

	protected:
		virtual bool on_client_connect(std::shared_ptr<CoCadNet::Connection<MessageTypes>> client) {
			CoCadNet::msg<MessageTypes> m;
			m.head.ID = MessageTypes::ServerAccept;
			client->send_msg(m);
			return true;
		}

		virtual void on_client_disconnect(std::shared_ptr<CoCadNet::Connection<MessageTypes>> client) {
			std::cout << "ACTION-[SERVER] Removing Client [" << client->get_id() << "]\n";
		}

		virtual void on_message_received(std::shared_ptr<CoCadNet::Connection<MessageTypes>> client, CoCadNet::msg<MessageTypes>& m) {
			
			// Process messages depending on the header type (aka message type)
			switch (m.head.ID) {
				case MessageTypes::ServerPing: {
					std::cout << "STATUS-[CLIENT:" << client->get_id() << "] Server Pinged..\n";
					client->send_msg(m); // send same ping packet back
				} break;

        case MessageTypes::ccRequestAuthentication: {
          CoCadNet::msg<MessageTypes> result;
          result.head.ID = MessageTypes::ccAuthenticationOutcome;
          
          bool success = false; 
          if (is_usr_data_loaded) {
            std::cout << "Checking For: " << m.dat[0] << " " << m.dat[1] << "\n";
            if (usr_acc_data.count(m.dat[0]) > 0) {
              // check if passwords match
              std::cout << "Checking..";
              if (m.dat[1] == usr_acc_data[m.dat[0]]) { success = true; }
            }
          }

          std::string res_val = success ? "1" : "0";
          std::string cl_id = std::to_string(client->get_id());

          result.dat.push_back(res_val);
          result.dat.push_back(cl_id);
          result.head.size = result.dat.size();
          std::cout << "STATUS-[SERVER] Authentication Request: CLIENT[" << cl_id << "] = " << res_val << "\n";
          client->send_msg(result);
        }

				/*
				case MessageTypes::MessageAll: {
					std::cout << "STATUS-[" << client->get_id() << "] Messaged All Clients\n";

					CoCadNet::msg<MessageTypes> m;
					m.head.ID = MessageTypes::ServerMessage; // clients treat it as server
					m.dat.push_back(std::string(client->get_id()));
					broadcast_msg(m, client);
				} break;*/
			}
		}

  private:
    std::map<std::string, std::string> usr_acc_data;
    bool is_usr_data_loaded = false;
    std::map<unsigned int, std::string> hosting_users;
};

int main()
{
	CoCadServer server(60000); 
	server.start_server();
  server.load_user_data("./data/users.acc");

	while (true) {
		server.update_server(20, true);
	}
	
	return 0;
}
