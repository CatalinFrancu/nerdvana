<?php

class Util {

  static function rangeExcept($start, $end, $except): array {
    return array_diff(range($start, $end), [ $except ]);
  }
}
