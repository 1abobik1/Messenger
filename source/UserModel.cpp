#include "../header/UserModel.h"

std::uint64_t UserModel::get_id() const { return user_id_; }

std::string UserModel::get_name() const { return name_; }

std::string UserModel::get_password() const { return password_; }

std::string UserModel::get_email() const { return email_; }


void UserModel::set_id(uint64_t user_id) { user_id_ = user_id; }

void UserModel::set_name(const std::string& name) { name_ = name; }

void UserModel::set_password(const std::string& password) { password_ = password; }

void UserModel::set_email(const std::string& email) { email_ = email; }
