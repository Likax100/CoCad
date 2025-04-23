#pragma once

#include <memory>
#include <mutex>
#include <deque>
#include <algorithm>
#include <iostream>
#include <thread>

#define ASIO_STANDALONE
#define _WIN32_WINDOWS 0x0A00 // min supported win-networking version (win 10)
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

namespace CoCadNet {
  // Message Handling
  // Message is sent as a packet of: Header + Body
  // Header has a unique ID which helps in decoding messages and their types
  template <typename T>
  struct msg_head { T ID{}; unsigned int size; };

  template <typename T>
  struct msg { msg_head<T> head{}; std::vector<uint8_t> data; };

  template <typename T>
  unsigned int size_of_msg(msg<T>& m) { return sizeof(msg_head<T>) + m.data.size(); }

  template <typename T>
  void add_to_body(msg<T>& m, const T& data) {
    unsigned int end_of_body = m.data.size();
    m.data.resize(m.data.size() + sizeof(T)); //resize to fit new data
    std::memcpy(m.data.data() + end_of_body, &data, sizeof(T)); // copy data to body - physically
    m.head.size = size_of_msg(m);
  }

  template <typename T>
  void pop_from_body(msg<T>& m, T& data_var) {
    unsigned int index = m.data.size() - sizeof(T);
    std::memcpy(&data_var, m.data.data() + index, sizeof(T));
    m.data.resize(index);
    m.head.size = size_of_msg(m);
  }

  template <typename T>
  class Connection;

  template <typename T>
  struct signed_msg { msg<T> m; std::shared_ptr<Connection<T>> connect_obj = nullptr; };
  
}
