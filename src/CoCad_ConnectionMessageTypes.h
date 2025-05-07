#pragma once

enum class MessageTypes : unsigned int {
  ServerAccept,
  ServerDeny,
  ServerPing,
  MessageAll,
  ServerMessage,
 
  ccRequestAuthentication,
  ccAuthenticationOutcome,

  ccRequestSessionHost,
  ccStatusHostExists,
  ccSessionHostRequestOutcome,
  ccCurrentHostList,

  ccRequestJoinSession,
  ccJoinSessionOutcome,

  ccOpRequestSHModelData,
  ccOpSHSentModelData,
  ccOpBroadcastModelChange,

};
