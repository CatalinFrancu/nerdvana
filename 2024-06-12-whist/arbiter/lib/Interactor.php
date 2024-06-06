<?php

class Interactor {
  const string INPUT_FILE = 'input.txt';
  const string OUTPUT_FILE = 'output.txt';
  const int TIMEOUT = 2; // secunde

  static function readOutputFile(array $choices): int {
    $contents = @file_get_contents(self::OUTPUT_FILE);

    if ($contents === false) {
      Log::warn('Fișierul %s nu există.', [ self::OUTPUT_FILE ]);
      throw new WException('Fișierul de ieșire nu există.');
    }

    $contents = trim($contents);
    if (!is_numeric($contents)) {
      Log::warn('Fișierul %s conține altceva decît un număr: [%s].',
                [ self::OUTPUT_FILE, $contents ]);
      throw new WException('Fișier de ieșire incorect.');
    }

    $choice = (int)$contents;
    if (!in_array($choice, $choices)) {
      Log::warn('Fișierul %s conține o valoare ilegală: [%d].',
                [ self::OUTPUT_FILE, $choice ]);
      throw new WException('Valoare incorectă.');
    }

    return $choice;
  }

  static function interact(string $binary, GameState $state, array $choices): int {
    $dir = dirname($binary);
    // Log::default('Apelez %s în directorul %s.', [ $binary, $dir ]);
    chdir($dir);
    @unlink(self::INPUT_FILE);
    @unlink(self::OUTPUT_FILE);
    $state->writeToFile(self::INPUT_FILE);

    $cmd = sprintf('ulimit -t %d && %s', self::TIMEOUT, $binary);
    $output = null;
    $resultCode = null;
    exec($cmd, $output, $resultCode);
    if ($resultCode !== 0) {
      Log::warn('Clientul s-a terminat cu codul %d.', [ $resultCode ]);
    }

    try {
      $choice = self::readOutputFile($choices);
    } catch (WException $e) {
      $choice = $choices[array_rand($choices)];
      Log::warn('Am ales la întîmplare varianta %d dintre cele corecte.',
                [ $choice ]);
    }
    return $choice;
  }
}
