#include "messenger/server.hpp"

#include <App.h>

#include "messenger/api.hpp"
#include "messenger/auth_service.hpp"
#include "messenger/chat.hpp"
#include "messenger/db/session_repository.hpp"
#include "messenger/log.hpp"
#include "messenger/services.hpp"

namespace messenger {

int runServer(const Config& config) {
    db::Connection connection(config.databaseUrl);
    db::UserRepository users(connection);
    db::SessionRepository sessions(connection);
    db::FriendRepository friends(connection);
    db::MessageRepository messages(connection);
    AuthService auth(users, sessions, config.sessionTtl);
    http::Responder responder(config.corsOrigin);
    Services services{connection, users, friends, messages, auth, responder};

    if (const int removed = sessions.removeExpired(); removed > 0) {
        log::info("removed " + std::to_string(removed) + " expired sessions");
    }

    uWS::App app;
    registerApi(app, services);
    ChatHandler chat(services);
    chat.registerOn(app);
    app.any("/*", [&responder](http::Response* res, http::Request*) {
        responder.error(res, http::kNotFound, "not found");
    });

    bool listening = false;
    app.listen(config.port, [&](us_listen_socket_t* socket) {
        listening = socket != nullptr;
        if (listening) {
            log::info("listening on port " + std::to_string(config.port));
        }
    });
    if (!listening) {
        log::error("cannot listen on port " + std::to_string(config.port));
        return 1;
    }
    app.run();
    return 0;
}

}  // namespace messenger
