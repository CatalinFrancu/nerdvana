<?php

class Deck {
  const int NUM_SUITS = 4;
  const int NUM_RANKS = 13;

  private array $cards;

  function __construct(int $numPlayers) {
    $minRank = self::NUM_RANKS - 2 * $numPlayers;
    $this->cards = [];
    for ($suit = 0; $suit < self::NUM_SUITS; $suit++) {
      for ($rank = $minRank; $rank < self::NUM_RANKS; $rank++) {
        $this->cards[] = new Card($suit * self::NUM_RANKS + $rank);
      }
    }
    shuffle($this->cards);
  }

  function dealTrump(): Card {
    if (empty($this->cards)) {
      return Card::none();
    }
    return array_shift($this->cards);
  }

  function dealHand(int $size): array {
    return array_splice($this->cards, 0, $size);
  }

  function __toString(): string {
    return Card::toString($this->cards);
  }
}
