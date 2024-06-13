#pragma once

#include <string>
#include <cstdint>


class UserModel
{
private:
    std::uint64_t user_id_;
    std::string name_;
    std::string password_;
    std::string email_;

public:
    UserModel() = default;

	UserModel(const uint64_t user_id, std::string name,std::string password,std::string email)
        : user_id_(user_id), name_(std::move(name)), password_(std::move(password)), email_(std::move(email)) {}

    std::uint64_t get_id() const;
    std::string get_name() const;
    std::string get_password() const;
    std::string get_email() const;

    void set_id(uint64_t user_id);
    void set_name(const std::string& name);
    void set_password(const std::string& password);
    void set_email(const std::string& email);
};