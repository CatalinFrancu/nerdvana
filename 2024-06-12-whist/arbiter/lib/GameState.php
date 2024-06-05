<?php

class GameState {
  public int $numPlayers;
  public int $playerId;
  public array $scores;
  public array $streaks;

  public int $roundId;
  public int $handSize;
  public Card $trump;
  public array $initialHand;

  public int $phase;
  public array $bids;

  public int $trickId;
  public array $tricks;

  function __construct(
    int $numPlayers,
    int $playerId,
    array $scores,
    array $streaks,
    int $roundId,
    int $handSize,
    Card $trump,
    array $initialHand,
    int $phase,
    array $bids,
  ) {
    $this->numPlayers = $numPlayers;
    $this->playerId = $playerId;
    $this->scores = $scores;
    $this->streaks = $streaks;
    $this->roundId = $roundId;
    $this->handSize = $handSize;
    $this->trump = $trump;
    $this->initialHand = $initialHand;
    $this->phase = $phase;
    $this->bids = $bids;
  }

  function getFirstBidder(): int {
    return $this->roundId % $this->numPlayers;
  }

  function getPlayerOrder(): int {
    $mod = ($this->playerId - $this->roundId) % $this->numPlayers;
    if ($mod < 0) {
      $mod += $this->numPlayers;
    }
    return $mod;
  }

  function getInitialHand(): string {
    $values = [];
    foreach ($this->initialHand as $c) {
      $values[] = $c->val;
    }
    return implode(' ', $values);
  }

  function writeToFile(string $filename): void {
    $lines = [];
    $lines[] = 'num_players ' . $this->numPlayers;
    $lines[] = 'player_id ' . $this->playerId;
    $lines[] = 'scores ' . implode(' ', $this->scores);
    $lines[] = 'streaks ' . implode(' ', $this->streaks);
    $lines[] = 'round_id ' . $this->roundId;
    $lines[] = 'hand_size ' . $this->handSize;
    $lines[] = 'first_bidder ' . $this->getFirstBidder();
    $lines[] = 'player_order ' . $this->getPlayerOrder();
    $lines[] = 'trump ' . $this->trump->val;
    $lines[] = 'initial_hand ' . $this->getInitialHand();
    $lines[] = 'phase ' . $this->phase;
    $lines[] = 'bids ' . implode(' ', $this->bids);

    if ($this->phase == Game::PHASE_PLAY) {
      $lines[] = 'trick_id ' .  $this->trickId;
      foreach ($this->tricks as $t) {
        $size = count($t->cards);
        $values = Util::objectProperty($t->cards, 'val');
        $l = 'trick ' . $size . ' ' . implode(' ', $values);
        $lines[] = trim($l);
        if ($t->hasWinner()) {
          $lines[] = 'winner ' . $t->winner;
        }
      }
    }

    $contents = implode("\n", $lines) . "\n";
    file_put_contents($filename, $contents);
  }
}
