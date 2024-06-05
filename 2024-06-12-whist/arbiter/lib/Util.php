<?php

class Util {

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
}
