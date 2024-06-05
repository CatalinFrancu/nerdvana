<?php

class Trick {
  const int NONE = -1;

  public array $cards = [];
  public int $winner;

  function __construct() {
    $this->winner = self::NONE;
  }

  function hasWinner(): bool {
    return ($this->winner != self::NONE);
  }
}
