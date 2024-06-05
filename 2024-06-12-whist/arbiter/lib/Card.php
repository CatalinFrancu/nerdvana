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
    $suit = intdiv($this->val, Deck::NUM_RANKS);
    $rank = $this->val % DECK::NUM_RANKS;
    return sprintf('%s%s%s%s',
                   self::COLORS[$suit],
                   self::NAMES[$rank],
                   self::SUITS[$suit],
                   AnsiColors::DEFAULT);
  }

  static function none(): Card {
    return new Card(self::NONE);
  }
}
