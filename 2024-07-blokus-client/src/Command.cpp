#include "Command.h"
#include <iostream>
#include <string>

Command::Command() {
  type = T_UNKNOWN;
}

void Command::readFromStdin() {
  std::string s;
  std::getline(std::cin, s);
  s = trim(s);
  std::string cmd = getToken(s);

  if (cmd == "set_game") {
    type = T_SET_GAME;
    arg = s;
  } else if (cmd == "clear_board") {
    type = T_CLEAR_BOARD;
    arg = "";
  } else if (cmd == "cputime") {
    type = T_CPUTIME;
    arg = "";
  } else if (cmd == "quit") {
    type = T_QUIT;
    arg = "";
  } else {
    type = T_UNKNOWN;
    arg = cmd;
  }
}

std::string Command::getToken(std::string& s) {
  int pos = 0, len = s.length();
  while ((pos < len) && !std::isspace(s[pos])) {
    pos++;
  }

  std::string first = s.substr(0, pos);
  s = trim(s.substr(pos));
  return first;
}

std::string Command::trim(std::string s) {
  int l = 0, r = s.length();

  while ((l < r) && std::isspace(s[l])) {
    l++;
  }
  while ((r > l) && std::isspace(s[r - 1])) {
    r--;
  }
  return s.substr(l, r - l);
}
