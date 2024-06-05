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

  function computeWinner(int $first, Card $trump): int {
    $best = 0;
    for ($i = 1; $i < count($this->cards); $i++) {
      if ($this->cards[$i]->beats($this->cards[$best], $trump)) {
        $best = $i;
      }
    }
    $this->winner = ($best + $first) % count($this->cards);
    return $this->winner;
  }
}
