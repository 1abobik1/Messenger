#include "../header/UserData.h"

uint64_t UserData::get_id() const { return user_id_; }

std::string UserData::get_name() const { return name_; }

std::string UserData::get_password() const { return password_; }

std::string UserData::get_email() const { return email_; }


void UserData::set_id(uint64_t user_id) { user_id_ = user_id; }

void UserData::set_name(const std::string& name) { name_ = name; }

void UserData::set_password(const std::string& password) { password_ = password; }

void UserData::set_email(const std::string& email) { email_ = email; }
