#pragma once

#include <App.h>

#include "messenger/services.hpp"

namespace messenger {

// REST API under /api (see README for the list of routes).
void registerApi(uWS::App& app, Services& services);

}  // namespace messenger
