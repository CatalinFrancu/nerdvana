#include <iostream>
#include "Response.h"

void Response::send(char header, const std::string& msg) {
  std::cout << header << " " << msg << "\n\n";
  std::cout.flush();
}

void Response::fail(const std::string& msg) {
  send('?', msg);
}

void Response::success(const std::string& msg) {
  send('=', msg);
}
