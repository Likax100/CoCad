#pragma once
#include <string>
#include "CoCad_Common.h"


namespace CoCadNet
{
	template <typename T>
	struct msg_head { T ID { }; unsigned int size = 0; };

	template <typename T>
	struct msg {
		msg_head<T> head { };
		std::string dat; // consider std::vector<std::string> ?

		unsigned int size() const { return dat.size(); }
	};

	template <typename T>
	class Connection; // forward declare so compiler knows it exists

	// message with attached client info - specialised message, needed for identification
	template <typename T>
	struct signed_msg { std::shared_ptr<Connection<T>> remote = nullptr; msg<T> m; };		

}

