<?php

class Card {
  const int NONE = -1;
  const array NAMES = [
    '2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K', 'A',
  ];
  const array SUITS = [ '♥', '♠', '♦', '♣' ];
  const array COLORS = [
    AnsiColors::ERROR, AnsiColors::BRIGHT,
    AnsiColors::ERROR, AnsiColors::BRIGHT,
  ];

  public int $val;

  function __construct(int $val) {
    $this->val = $val;
  }

  function getSuit() {
    return intdiv($this->val, Deck::NUM_RANKS);
  }

  function getRank() {
    return $this->val % Deck::NUM_RANKS;
  }

  static function toString(array $cards): string {
    $msg = '';
    foreach ($cards as $c) {
      $msg .= $c . ' ';
    }

    return '<' . trim($msg) . '>';
  }

  function __toString(): string {
    if ($this->val == self::NONE) {
      return '---';
    }
    return sprintf('%s%s%s%s',
                   self::COLORS[$this->getSuit()],
                   self::NAMES[$this->getRank()],
                   self::SUITS[$this->getSuit()],
                   AnsiColors::DEFAULT);
  }

  static function none(): Card {
    return new Card(self::NONE);
  }

  function isNone(): bool {
    return $this->val == self::NONE;
  }
}
