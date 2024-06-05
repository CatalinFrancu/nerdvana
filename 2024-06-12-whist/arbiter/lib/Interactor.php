<?php

class Interactor {
  const INPUT_FILE = 'input.txt';
  const OUTPUT_FILE = 'output.txt';

  static function interact(string $binary, GameState $state, array $choices): int {
    $dir = dirname($binary);
    chdir($dir);
    @unlink(self::INPUT_FILE);
    @unlink(self::OUTPUT_FILE);
    $state->writeToFile(self::INPUT_FILE);
    exec($binary);
    //    var_dump(file_get_contents(self::OUTPUT_FILE));
    return 0;
  }
}
