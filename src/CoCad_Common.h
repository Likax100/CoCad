#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <deque>
#include <optional> // DBR
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstdint>

//ASIO implementation requires it, just defines what min networking windows version program supports
#define _WIN32_WINNT 0x0A00 // 0x0A00 = windows version 10 

#define ASIO_STANDALONE // we DO NOT want anything to do with boost, just ASIO
#include <asio.hpp>
#include <asio/ts/buffer.hpp> 
#include <asio/ts/internet.hpp>
