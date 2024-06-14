#pragma once

#include "messenger/config.hpp"

namespace messenger {

// Connects to the database, registers the HTTP API and the chat, and runs the event loop.
// Returns a process exit code.
int runServer(const Config& config);

}  // namespace messenger
