#include "messenger/api.hpp"

#include <utility>

#include "messenger/log.hpp"

namespace messenger {

using http::Json;
using http::Request;
using http::Response;

namespace {

constexpr int kDefaultPageSize = 50;
constexpr int kMaxPageSize = 100;

Json sessionJson(const Session& session) {
    return Json{{"token", session.token},
                {"user", {{"id", session.user.id}, {"username", session.user.username}, {"email", session.user.email}}}};
}

std::string_view statusFor(AuthFailure::Reason reason) {
    switch (reason) {
        case AuthFailure::Reason::InvalidInput:
            return http::kBadRequest;
        case AuthFailure::Reason::EmailTaken:
            return http::kConflict;
        case AuthFailure::Reason::InvalidCredentials:
            return http::kUnauthorized;
    }
    return http::kBadRequest;
}

void replyAuth(Services& s, Response* res, const AuthOutcome& outcome, std::string_view successStatus) {
    if (const auto* session = std::get_if<Session>(&outcome)) {
        s.responder.json(res, successStatus, sessionJson(*session));
    } else {
        const auto& failure = std::get<AuthFailure>(outcome);
        s.responder.error(res, statusFor(failure.reason), failure.message);
    }
}

// Runs a synchronous handler and turns unexpected exceptions into a 500 response.
template <class Handler>
auto guarded(Services& s, Handler handler) {
    return [&s, handler = std::move(handler)](Response* res, Request* req) mutable {
        try {
            handler(res, req);
        } catch (const std::exception& e) {
            log::error(std::string("request failed: ") + e.what());
            s.responder.error(res, http::kInternalError, "internal server error");
        }
    };
}

// Like guarded(), but first resolves the session token; the handler gets the id of the signed-in user.
// The user always comes from the token, never from the request body.
template <class Handler>
auto authed(Services& s, Handler handler) {
    return guarded(s, [&s, handler = std::move(handler)](Response* res, Request* req) mutable {
        const auto userId = s.auth.authenticate(http::bearerToken(req));
        if (!userId) {
            s.responder.error(res, http::kUnauthorized, "authentication required");
            return;
        }
        handler(res, req, *userId);
    });
}

}  // namespace

Json toJson(const UserSummary& user) { return Json{{"id", user.id}, {"username", user.username}}; }

Json toJson(const Message& message) {
    return Json{{"id", message.id},
                {"from", message.from},
                {"to", message.to},
                {"body", message.body},
                {"sent_at", message.sentAt}};
}

void registerApi(uWS::App& app, Services& s) {
    app.post("/api/auth/signup", guarded(s, [&s](Response* res, Request*) {
        s.responder.readJson(res, [&s, res](Json body) {
            const auto username = http::jsonString(body, "username");
            const auto email = http::jsonString(body, "email");
            const auto password = http::jsonString(body, "password");
            if (!username || !email || !password) {
                s.responder.error(res, http::kBadRequest, "username, email and password are required");
                return;
            }
            replyAuth(s, res, s.auth.signup(*username, *email, *password), http::kCreated);
        });
    }));

    app.post("/api/auth/login", guarded(s, [&s](Response* res, Request*) {
        s.responder.readJson(res, [&s, res](Json body) {
            const auto email = http::jsonString(body, "email");
            const auto password = http::jsonString(body, "password");
            if (!email || !password) {
                s.responder.error(res, http::kBadRequest, "email and password are required");
                return;
            }
            replyAuth(s, res, s.auth.login(*email, *password), http::kOk);
        });
    }));

    app.post("/api/auth/logout", guarded(s, [&s](Response* res, Request* req) {
        s.auth.logout(http::bearerToken(req));
        s.responder.noContent(res);
    }));

    app.get("/api/me", authed(s, [&s](Response* res, Request*, UserId me) {
        const auto user = s.users.findById(me);
        if (!user) {
            s.responder.error(res, http::kUnauthorized, "authentication required");
            return;
        }
        s.responder.json(res, http::kOk, {{"id", user->id}, {"username", user->username}, {"email", user->email}});
    }));

    app.get("/api/users/search", authed(s, [&s](Response* res, Request* req, UserId) {
        const std::string email(req->getQuery("email"));
        if (email.empty()) {
            s.responder.error(res, http::kBadRequest, "query parameter 'email' is required");
            return;
        }
        const auto user = s.users.findByEmail(email);
        if (!user) {
            s.responder.error(res, http::kNotFound, "user not found");
            return;
        }
        s.responder.json(res, http::kOk, toJson(UserSummary{user->id, user->username}));
    }));

    app.get("/api/users/:id", authed(s, [&s](Response* res, Request* req, UserId) {
        const auto id = http::parseId(req->getParameter(0));
        const auto user = id ? s.users.findById(*id) : std::nullopt;
        if (!user) {
            s.responder.error(res, http::kNotFound, "user not found");
            return;
        }
        s.responder.json(res, http::kOk, toJson(UserSummary{user->id, user->username}));
    }));

    app.get("/api/friends", authed(s, [&s](Response* res, Request*, UserId me) {
        Json friends = Json::array();
        for (const UserSummary& user : s.friends.list(me)) {
            friends.push_back(toJson(user));
        }
        s.responder.json(res, http::kOk, friends);
    }));

    app.post("/api/friends", authed(s, [&s](Response* res, Request*, UserId me) {
        s.responder.readJson(res, [&s, res, me](Json body) {
            const auto friendId = http::jsonId(body, "friend_id");
            if (!friendId) {
                s.responder.error(res, http::kBadRequest, "friend_id must be a positive integer");
                return;
            }
            if (*friendId == me) {
                s.responder.error(res, http::kBadRequest, "you cannot add yourself as a friend");
                return;
            }
            const auto user = s.users.findById(*friendId);
            if (!user) {
                s.responder.error(res, http::kNotFound, "user not found");
                return;
            }
            const bool added = s.friends.add(me, *friendId);
            s.responder.json(res, added ? http::kCreated : http::kOk, toJson(UserSummary{user->id, user->username}));
        });
    }));

    app.get("/api/messages/:peerId", authed(s, [&s](Response* res, Request* req, UserId me) {
        const auto peerId = http::parseId(req->getParameter(0));
        if (!peerId) {
            s.responder.error(res, http::kBadRequest, "invalid user id");
            return;
        }

        std::optional<MessageId> before;
        if (const std::string_view raw = req->getQuery("before"); !raw.empty()) {
            before = http::parseId(raw);
            if (!before) {
                s.responder.error(res, http::kBadRequest, "'before' must be a message id");
                return;
            }
        }
        int limit = kDefaultPageSize;
        if (const std::string_view raw = req->getQuery("limit"); !raw.empty()) {
            const auto parsed = http::parseId(raw);
            if (!parsed || *parsed > kMaxPageSize) {
                s.responder.error(res, http::kBadRequest, "'limit' must be between 1 and 100");
                return;
            }
            limit = static_cast<int>(*parsed);
        }

        Json messages = Json::array();
        for (const Message& message : s.messages.conversation(me, *peerId, before, limit)) {
            messages.push_back(toJson(message));
        }
        s.responder.json(res, http::kOk, messages);
    }));

    app.get("/api/health", guarded(s, [&s](Response* res, Request*) {
        try {
            s.connection.exec("SELECT 1");
            s.responder.json(res, http::kOk, {{"status", "ok"}});
        } catch (const db::DbError&) {
            s.responder.json(res, http::kServiceUnavailable, {{"status", "database unavailable"}});
        }
    }));

    app.options("/*", [&s](Response* res, Request*) { s.responder.preflight(res); });
}

}  // namespace messenger
