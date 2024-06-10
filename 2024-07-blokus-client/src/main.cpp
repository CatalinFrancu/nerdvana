#include "Command.h"
#include "Game.h"
#include "Response.h"

int main() {

  Command cmd;
  Game game;

  while (cmd.type != Command::T_QUIT) {
    cmd.readFromStdin();
    switch (cmd.type) {
      case Command::T_SET_GAME:
        if (game.setType(cmd.arg)) {
          Response::success("");
        } else {
          Response::fail("Unknown game type [" + cmd.arg + "].");
        }
        break;

      case Command::T_CLEAR_BOARD:
        game.restart();
        Response::success("");
        break;

      case Command::T_CPUTIME:
        // Unimplemented.
        Response::success("0.0");
        break;

      case Command::T_UNKNOWN:
        Response::fail("Unknown command [" + cmd.arg + "].");
        break;
    }
  }

  Response::success("");
  return 0;
}
