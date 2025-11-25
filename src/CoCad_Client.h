#pragma once
#include <iostream>
#include "CoCad_Networking.h"
#include "CoCad_ConnectionMessageTypes.h"
#include "Editor.h"

class CoCadClient : public CoCadNet::i_Client<MessageTypes> {
	public:
		void PingServer(float t = -1.0f) {
			CoCadNet::msg<MessageTypes> m;
			m.head.ID = MessageTypes::ServerPing;	
      
      std::string m_body;
      if (t != -1.0f) {
        m_body = std::to_string(t);
        m.dat = m_body;
        m.head.size = m.dat.size();
      }

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

    void UpdateAllConnectedClientEditorRepr(EditorRepr& repr) {
      CoCadNet::msg<MessageTypes> repr_dat;
      repr_dat.head.ID = MessageTypes::ccOpBroadcastModelChange;

      repr_dat.dat += "\nogvc";
      for (int ogvc_i = 0; ogvc_i < Editor::repr.og_vert_cpy.size(); ogvc_i++) { 
        repr_dat.dat += " " + std::to_string(Editor::repr.og_vert_cpy[ogvc_i]); 
      }
            
      repr_dat.dat += "\nuv";
      for (int uv_i = 0; uv_i < Editor::repr.unique_verts.size(); uv_i++) { 
         repr_dat.dat += " " + std::to_string(Editor::repr.unique_verts[uv_i]); 
      }
            
      repr_dat.dat += "\nufn";
      for (int ufn_i = 0; ufn_i < Editor::repr.unique_face_normals.size(); ufn_i++) {
        repr_dat.dat += " " + std::to_string(Editor::repr.unique_face_normals[ufn_i]); 
      }
            
      repr_dat.dat += "\nfi";
      for (int fi_i = 0; fi_i < Editor::repr.face_indices.size(); fi_i++) { 
        repr_dat.dat += " " + std::to_string(Editor::repr.face_indices[fi_i]); 
      }
            
      repr_dat.dat += "\nnr";
      for (int nr_i = 0; nr_i < Editor::repr.normals_redundant.size(); nr_i++) { 
        repr_dat.dat += " " + std::to_string(Editor::repr.normals_redundant[nr_i]); 
      }
          
      repr_dat.head.size = repr_dat.dat.size();
      send_msg(repr_dat);
    }

};

