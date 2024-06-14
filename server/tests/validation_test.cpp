#include "messenger/validation.hpp"

#include <gtest/gtest.h>

#include <string>

using namespace messenger::validation;

TEST(Validation, Utf8) {
    EXPECT_TRUE(isValidUtf8("hello"));
    EXPECT_TRUE(isValidUtf8("Привет 👋"));
    EXPECT_FALSE(isValidUtf8("\xC3"));              // truncated
    EXPECT_FALSE(isValidUtf8("\xC0\xAF"));          // overlong '/'
    EXPECT_FALSE(isValidUtf8("\xED\xA0\x80"));      // surrogate
    EXPECT_FALSE(isValidUtf8("\xF4\x90\x80\x80"));  // above U+10FFFF
    EXPECT_EQ(utf8Length("Привет 👋"), 8u);
}

TEST(Validation, NormalizeEmail) {
    EXPECT_EQ(normalizeEmail("  Bob@Mail.RU \n"), "bob@mail.ru");
}

TEST(Validation, Username) {
    EXPECT_FALSE(validateUsername("alice"));
    EXPECT_FALSE(validateUsername("Дмитрий"));
    EXPECT_TRUE(validateUsername(""));
    EXPECT_TRUE(validateUsername("   "));
    EXPECT_TRUE(validateUsername("12345"));
    EXPECT_FALSE(validateUsername(std::string(20, 'x')));
    EXPECT_TRUE(validateUsername(std::string(21, 'x')));
}

TEST(Validation, Email) {
    EXPECT_FALSE(validateEmail("a@b.co"));
    EXPECT_FALSE(validateEmail(" A@B.CO "));
    EXPECT_TRUE(validateEmail("no-at-sign"));
    EXPECT_TRUE(validateEmail("@b.co"));
    EXPECT_TRUE(validateEmail("a@b"));
    EXPECT_TRUE(validateEmail("a@b.co."));
    EXPECT_TRUE(validateEmail("a@@b.co"));
    EXPECT_TRUE(validateEmail("a b@c.co"));
}

TEST(Validation, Password) {
    EXPECT_TRUE(validatePassword("short"));
    EXPECT_FALSE(validatePassword("0123456789"));
    EXPECT_FALSE(validatePassword("пароль1234"));  // 10 characters, 16 bytes
    EXPECT_TRUE(validatePassword(std::string(73, 'x')));
}

TEST(Validation, MessageBody) {
    EXPECT_FALSE(validateMessageBody("hi"));
    EXPECT_TRUE(validateMessageBody(""));
    EXPECT_TRUE(validateMessageBody(" \n\t "));
    EXPECT_FALSE(validateMessageBody(std::string(4000, 'x')));
    EXPECT_TRUE(validateMessageBody(std::string(4001, 'x')));
    EXPECT_TRUE(validateMessageBody("\xFF"));
}
