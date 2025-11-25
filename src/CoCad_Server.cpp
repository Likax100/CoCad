#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "CoCad_Networking.h"
#include "CoCad_ConnectionMessageTypes.h"
#include "CoCad_Utils.h"
#include "RandomGenerators.h"

typedef struct {
  int host_ID;
  std::string host_name;
  unsigned int n_connected_clients;
  std::vector<int> connected_IDs;
} HostInfo;

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

        // Set Up alphanumerictable - theres definetly a better way but this should work for now
        alpha_numeric_table[0] = "a";
        alpha_numeric_table[1] = "b";
        alpha_numeric_table[2] = "c";
        alpha_numeric_table[3] = "d";
        alpha_numeric_table[4] = "e";
        alpha_numeric_table[5] = "f";
        alpha_numeric_table[6] = "g";
        alpha_numeric_table[7] = "h";
        alpha_numeric_table[8] = "i";
        alpha_numeric_table[9] = "j";
        alpha_numeric_table[10] = "k";
        alpha_numeric_table[11] = "l";
        alpha_numeric_table[12] = "m";
        alpha_numeric_table[13] = "n";
        alpha_numeric_table[14] = "o";
        alpha_numeric_table[15] = "p";
        alpha_numeric_table[16] = "q";
        alpha_numeric_table[17] = "r";
        alpha_numeric_table[18] = "s";
        alpha_numeric_table[19] = "t";
        alpha_numeric_table[20] = "u";
        alpha_numeric_table[21] = "v";
        alpha_numeric_table[22] = "w";
        alpha_numeric_table[23] = "x";
        alpha_numeric_table[24] = "y";
        alpha_numeric_table[25] = "z";
        alpha_numeric_table[26] = "1";
        alpha_numeric_table[27] = "2";
        alpha_numeric_table[28] = "3";
        alpha_numeric_table[29] = "4";
        alpha_numeric_table[30] = "5";
        alpha_numeric_table[31] = "6";
        alpha_numeric_table[32] = "7";
        alpha_numeric_table[33] = "8";
        alpha_numeric_table[34] = "9";
        alpha_numeric_table[35] = "0";

      } else { std::cout << "ERROR-[SERVER] Could Not Load Users (file must be of type .acc)\n";   }
    }

	protected:
		virtual bool on_client_connect(std::shared_ptr<CoCadNet::Connection<MessageTypes>> client) {
			CoCadNet::msg<MessageTypes> m;
			m.head.ID = MessageTypes::ServerAccept;
			client->send_msg(m);

      CoCadNet::msg<MessageTypes> m2;
      m2.head.ID = MessageTypes::ccCurrentHostList;

      std::string m2_body;
      for (auto session : hosted_sessions) {
        m2_body += std::to_string(session.second.host_ID) + " ";
        m2_body += session.second.host_name + " ";
        m2_body += session.first + " ";
      }
      m2.dat = m2_body;
      m2.head.size = m2.dat.size();
      
      client->send_msg(m2);
			return true;
		}

		virtual void on_client_disconnect(std::shared_ptr<CoCadNet::Connection<MessageTypes>> client) {
			std::cout << "ACTION-[SERVER] Removing Client [" << client->get_id() << "]\n";
		}

		virtual void on_message_received(std::shared_ptr<CoCadNet::Connection<MessageTypes>> client, CoCadNet::msg<MessageTypes>& m) {
			
			// Process messages depending on the header type (aka message type)
			auto packet_data = StringUtil::SplitString(m.dat, " ");

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
            int n_occurences = std::count(logged_in_users.begin(), logged_in_users.end(), packet_data[0]);
            if (usr_acc_data.count(packet_data[0]) > 0 && n_occurences == 0) {
              // check if passwords match
              if (packet_data[1] == usr_acc_data[packet_data[0]]) { success = true; }
            }
          }

          std::string res_val = success ? "1" : "0";
          std::string cl_id = std::to_string(client->get_id());

          result.dat = res_val + " " + cl_id;
          result.head.size = result.dat.size();
          std::cout << "STATUS-[SERVER] Authentication Request: CLIENT[" << cl_id << "] = " << res_val << "\n";
          
          if (success) { logged_in_users.push_back(packet_data[0]); }

          client->send_msg(result);
        } break;

        case MessageTypes::ccRequestSessionHost: {
         
          CoCadNet::msg<MessageTypes> outcome;
          outcome.head.ID = MessageTypes::ccSessionHostRequestOutcome;

          bool hosting_already = false;
          for (auto d : hosted_sessions) {
            if (d.second.host_ID == client->get_id()) { hosting_already = true; break; }
          }

          if (!hosting_already) {
            // gen random session key
            bool unique_key_found = false;
            std::string session_key;
            
            while (!unique_key_found) {
              session_key = "";
              for (unsigned int i = 0; i < 8; i++) {
                int ran_num = sesh_key_gen(ran_eng);
                session_key += alpha_numeric_table[ran_num];
              }

              //std::cout << "[DEBUG] KEY: " + session_key + "\n";

              // loop through current sessions ensure no keys match
              unique_key_found = true;
              for (auto sesh_key : hosted_sessions) {
                bool not_matches = sesh_key.first != session_key ? true : false;
                unique_key_found &= not_matches;
              }
            }

            if (unique_key_found) {
              HostInfo hi_obj;
              hi_obj.host_ID = client->get_id();
              hi_obj.host_name = packet_data[0];
              hi_obj.n_connected_clients = 0;
              hosted_sessions[session_key] = hi_obj;
            
              outcome.dat = "1";
              outcome.head.size = outcome.dat.size();
              client->send_msg(outcome);

              CoCadNet::msg<MessageTypes> host_status;
              host_status.head.ID = MessageTypes::ccStatusHostExists;
              std::string m_body = std::to_string(client->get_id()) + " " + packet_data[0] + " " + session_key;
              host_status.dat = m_body;
              //std::cout << "[DEBUG] CC_STATUS_HOST_EXISTS: " << m_body << "\n"; 
              host_status.head.size = host_status.dat.size();
              broadcast_msg(host_status, client);

              std::cout << "STATUS-[SERVER] Host Update: Session (" + session_key + ") is active, by " + packet_data[0] + ".\n";  
            } else { outcome.dat = "0"; }

          }
          
          outcome.head.size = outcome.dat.size();
          client->send_msg(outcome);

        } break;

        case MessageTypes::ccRequestJoinSession: {
          CoCadNet::msg<MessageTypes> m;
          m.head.ID = MessageTypes::ccJoinSessionOutcome;

          bool key_valid = hosted_sessions.count(packet_data[0]) > 0 ? true : false;

          //std::cout << "[DEBUG] " << packet_data[0] << "\n";

          std::string m_body;
          if (key_valid) {
            m_body = "1 ";
            m_body += std::to_string(hosted_sessions[packet_data[0]].host_ID) + " ";
            m_body += hosted_sessions[packet_data[0]].host_name + " ";
            hosted_sessions[packet_data[0]].n_connected_clients++;
            hosted_sessions[packet_data[0]].connected_IDs.push_back(std::stoi(std::to_string(client->get_id())));
          } else { m_body = "0"; }
         
          m.dat = m_body;
          m.head.size = m.dat.size();
          client->send_msg(m);
        } break;

        case MessageTypes::ccOpRequestSHModelData: {
          CoCadNet::msg<MessageTypes> m;
          m.head.ID = MessageTypes::ccOpRequestSHModelData;

          std::string m_body = "-1";
          for (auto session : hosted_sessions) {
            int count = std::count(session.second.connected_IDs.begin(), session.second.connected_IDs.end(), (int)client->get_id());
            if (count > 0) {
              m_body = "";
              m_body += std::to_string(session.second.host_ID) + " ";
              m_body += std::to_string(client->get_id());
              break;
            }
          }

          m.dat = m_body;
          m.head.size = m.dat.size();

          broadcast_msg(m, client); 
        } break;

        case MessageTypes::ccOpSHSentModelData: {
          //std::cout << "SIZE OF SERVER SENT MDL DATA: " << m.dat.size();
          CoCadNet::msg<MessageTypes> n;
          n.head.ID = MessageTypes::ccOpSHSentModelData;
          n.dat = m.dat;
          n.head.size = n.dat.size();
          broadcast_msg(n, client);
        } break;

        case MessageTypes::ccOpBroadcastModelChange: {
          CoCadNet::msg<MessageTypes> n;
          n.head.ID = MessageTypes::ccOpBroadcastModelChange;
          
          std::string n_body;
          for (auto session : hosted_sessions) {
            if (session.second.host_ID == (int)client->get_id()) {
              for (auto v = 0; v < session.second.connected_IDs.size(); v++) {
                n_body += std::to_string(session.second.connected_IDs[v]) + " ";
              }
            }
          }
          n_body += "\n";

          n.dat = n_body + m.dat;
          n.head.size = n.dat.size();
          broadcast_msg(n, client);
        } break;

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
    std::map<int, std::string> alpha_numeric_table;

    std::map<std::string, std::string> usr_acc_data;
    std::vector<std::string> logged_in_users;
    std::map<std::string, HostInfo> hosted_sessions;

    std::uniform_int_distribution<> sesh_key_gen = iRanGen(0,35);

    bool is_usr_data_loaded = false;
    //unsigned int session_host;
    //bool session_host_set = false;
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
