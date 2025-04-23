#pragma once

enum class MessageTypes : unsigned int {
  ServerAccept,
  ServerDeny,
  ServerPing,
  MessageAll,
  ServerMessage,
 
  ccRequestAuthentication,
  ccAuthenticationOutcome,
};
