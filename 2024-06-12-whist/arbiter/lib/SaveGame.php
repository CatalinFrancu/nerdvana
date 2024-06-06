<?php

class SaveGame {
  private string $fileName;
  private array $lines;

  function __construct(string $fileName) {
    $this->fileName = getcwd() . DIRECTORY_SEPARATOR . $fileName;
    $this->lines = [];
  }

  function addPlayers(array $players): void {
    $this->lines[] = 'num_players ' . count($players);
    foreach ($players as $p) {
      $this->lines[] = 'player_name ' . $p->name;
    }
  }

  function addRoundInfo(int $roundId, int $firstBidder, int $handSize): void {
    $this->lines[] = 'round_id ' . $roundId;
    $this->lines[] = 'first_bidder ' . $firstBidder;
    $this->lines[] = 'hand_size ' . $handSize;
  }

  function addHand(array $hand): void {
    $values = Util::objectProperty($hand, 'val');
    $this->lines[] = 'hand ' . implode(' ', $values);
  }

  function addTrump(Card $trump): void {
    $this->lines[] = 'trump ' . $trump->val;
  }

  function addBids(array $bids): void {
    $this->lines[] = 'bids ' . implode(' ', $bids);
  }

  function addTrick(Trick $t): void {
    $values = Util::objectProperty($t->cards, 'val');
    $this->lines[] = 'trick ' . implode(' ', $values);
    $this->lines[] = 'trick_winner ' . $t->winner;
  }

  function addOutcomes(array $outcomes): void {
    $this->lines[] = 'outcomes ' . implode(' ', $outcomes);
  }

  function addPoints(array $points): void {
    $this->lines[] = 'points ' . implode(' ', $points);
  }

  function addStreaks(array $streaks): void {
    $this->lines[] = 'streaks ' . implode(' ', $streaks);
  }

  function write() {
    $contents = implode("\n", $this->lines) .  "\n";
    file_put_contents($this->fileName, $contents);
  }
}
