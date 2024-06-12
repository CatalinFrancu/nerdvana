<?php

class Util {

  const int BASE_POINTS = 5;
  const int BONUS_STREAK = 5;
  const int BONUS_POINTS = 10;

  static function rangeExcept($start, $end, $except): array {
    return array_diff(range($start, $end), [ $except ]);
  }

  /* Returnează $obj->$prop pentru fiecare $obj în $a */
  static function objectProperty(array $a, string $prop): array {
    $results = [];
    foreach ($a as $obj) {
      $results[] = $obj->$prop;
    }
    return $results;
  }

  static function updateStreak(int $old, int $madeIt): int {
    if ($madeIt) {
      return ($old > 0) ? ($old + 1) : 1;
    } else {
      return ($old < 0) ? ($old - 1) : -1;
    }
  }

  static function getPoints(int $bid, int $taken, int $streak): int {
    $trickPoints = ($bid == $taken)
      ? (self::BASE_POINTS + $bid)
      : -abs($bid - $taken);
    $bonusPoints = ($streak == self::BONUS_STREAK)
      ? self::BONUS_POINTS
      : (($streak == -self::BONUS_STREAK) ? -self::BONUS_POINTS : 0);
    return $trickPoints + $bonusPoints;
  }

  static function genPermutation(int $n, bool $randomize): array {
    $p = range(0, $n - 1);
    if ($randomize) {
      shuffle($p);
    }
    return $p;
  }
}
