#define _CRT_SECURE_NO_WARNINGS

#include "../header/MessagerHandler.h"
#include <iostream>

#include "../header/Server.h"
#include "../header/CommonConst.h"
#include "../header/TimeUtils.h"

using namespace JsonChat;

void MessagerHandler::ConnectedUser(web_socket* WS) {
    std::cout << "\nConnectedUser__Start..\n";

    WebSocketUser* web_socket_user = WS->getUserData();
    web_socket_user->email_sender_id_ = request_handler_->getUserModel()->get_email();
    std::cout << "web_socket_user->email_: " << web_socket_user->email_sender_id_ << '\n';

    if (Database::getDatabase()->getUserTable()->CheckEmailExists(web_socket_user->email_sender_id_)) {
        web_socket_user->sender_id_ = Database::getDatabase()->getUserTable()->GetUserIdByEmail(web_socket_user->email_sender_id_);
        std::cout << "User connected ID: " << web_socket_user->sender_id_ << '\n';
    }
    else {
        std::string errorMessage = "User with email " + web_socket_user->email_sender_id_ + " not found ";
        std::cout << errorMessage;
    }

    WS->subscribe("userN" + std::to_string(web_socket_user->sender_id_));
    std::cout << "ConnectedUser__END.." << '\n';
}

bool MessagerHandler::CheckAndHandleBlock(web_socket* WS) {
    WebSocketUser* web_socket_user = WS->getUserData();

    auto now = std::chrono::steady_clock::now();
    if (web_socket_user->is_blocked) {
        if (now < web_socket_user->unblock_time) {
            std::cout << "User " << web_socket_user->sender_id_ << " is still blocked from sending messages.\n";
            std::cout << "Current time: " << std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
            std::cout << " unblock_time: " << std::chrono::duration_cast<std::chrono::seconds>(web_socket_user->unblock_time.time_since_epoch()).count() << '\n';

            json block_response;
            block_response[COMMAND] = "BLOCKED";
            block_response[MESSAGE] = "You are blocked from sending messages for 30 seconds.";
            WS->send(block_response.dump(), uWS::OpCode::TEXT);

            return true;  
        }
        else {
            std::cout << "Unblock the user\n";
            web_socket_user->is_blocked = false; 
        }
    }
    return false; 
}


void MessagerHandler::RecordMessageTime(web_socket* WS) {
    WebSocketUser* web_socket_user = WS->getUserData();
    web_socket_user->message_times.push_back({ time_utils::getMinutes(web_socket_user->sent_at), time_utils::getSeconds(web_socket_user->sent_at) });
    if (const int msg_counter_spam_detection = 8; web_socket_user->message_times.size() > msg_counter_spam_detection) {
        web_socket_user->message_times.pop_front();
    }

}

bool MessagerHandler::CheckAndHandleSpam(web_socket* WS) {
    WebSocketUser* web_socket_user = WS->getUserData();

    if (const int msg_counter_spam_detection = 8; web_socket_user->message_times.size() == msg_counter_spam_detection) {
        int sum_time_diff = 0;

        bool all_minute_equal = true;
        // if the minutes of sending the first two messages from the user are different, then you should not check
        for (size_t i = 0; i < web_socket_user->message_times.size() - 1; ++i) {
            if (web_socket_user->message_times[i].first != web_socket_user->message_times[i + 1].first) {
                all_minute_equal = false;
                break;
            }
        }

        
        if (all_minute_equal) {
            for (size_t i = 0; i < web_socket_user->message_times.size() - 1; ++i) {
                sum_time_diff += (web_socket_user->message_times[i + 1].second - web_socket_user->message_times[i].second);
            }
        }

        const int min_spam_time_in_sec = 9;
        if (web_socket_user->message_times.size() == 8 && sum_time_diff != 0 && sum_time_diff <= min_spam_time_in_sec)// if the user sent 8 messages faster than 10 seconds, he will be blocked for spam  
        {
            std::cout << "sum_time_diff " << sum_time_diff << '\n';
            web_socket_user->is_blocked = true;
            web_socket_user->unblock_time = std::chrono::steady_clock::now() + std::chrono::seconds(30);
            
            std::cout << "User " << web_socket_user->sender_id_ << " has been blocked for spamming.\n";

            json block_response;
            block_response[COMMAND] = "BLOCKED";
            block_response[MESSAGE] = "You are blocked from sending messages for 30 seconds.";
            WS->send(block_response.dump(), uWS::OpCode::TEXT);

            return true;
        }
    }
    return false;
}

void MessagerHandler::SendMsg(web_socket* WS) {
    WebSocketUser* web_socket_user = WS->getUserData();

    json response;
    response[COMMAND] = PRIVATE_MSG;
    response[SENDER_ID] = web_socket_user->receiver_id_;
    response[MESSAGE] = web_socket_user->message_;
    response[SENT_AT] = web_socket_user->sent_at;

    WS->publish("userN" + std::to_string(web_socket_user->receiver_id_), response.dump());
}

void MessagerHandler::ProcessPrivateMessage(web_socket* WS, json parsed) {
    std::cout << "\nstart ProcessPrivateMessage..\n";

    WebSocketUser* web_socket_user = WS->getUserData();

    // if the user is still blocked due to spam
    if (CheckAndHandleBlock(WS)) {
        return;
    }

    web_socket_user->message_ = parsed[MESSAGE];
    web_socket_user->receiver_id_ = parsed[RECEIVER_ID];
    web_socket_user->sent_at = Database::getDatabase()->getMessageTable()->InsertAndGetSentAt(web_socket_user->sender_id_, web_socket_user->receiver_id_, web_socket_user->message_);

    RecordMessageTime(WS);

    // Check if the user is spamming
    if (CheckAndHandleSpam(WS)) {
        return;
    }

    // if the user is not blocked, then he can send a message
    if (!CheckAndHandleSpam(WS) && !CheckAndHandleBlock(WS)) {
        SendMsg(WS);
    }

    std::cout << "end ProcessPrivateMessage..\n";
}

void MessagerHandler::ProcessMessage(web_socket* WS, std::string_view message) {
    std::cout << "\nMessage from user ID: " << WS->getUserData()->sender_id_ << "--message: " << message << '\n';

    auto parsed = json::parse(message);
    if (parsed[COMMAND] == PRIVATE_MSG) {
        ProcessPrivateMessage(WS, parsed);
    }
}

void MessagerHandler::DisconnectedUser(web_socket* WS, int code, std::string_view message) {
    std::cout << "\nUser disconnected: " << WS->getUserData()->sender_id_ << '\n';
}
