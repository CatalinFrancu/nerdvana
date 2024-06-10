#pragma once

#include <string>

class Response {
  static void send(char header, const std::string& msg);

public:
  static void fail(const std::string& msg);
  static void success(const std::string& msg);
};
