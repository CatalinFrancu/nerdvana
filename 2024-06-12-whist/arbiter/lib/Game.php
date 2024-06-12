<?php

class Game {
  const int MIN_PLAYERS = 3;
  const int MAX_PLAYERS = 6;
  const int PHASE_BID = 0;
  const int PHASE_PLAY = 1;

  const array HAND_SIZES = [
    3 => [ 1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8,
           8, 8, 8, 7, 6, 5, 4, 3, 2, 1, 1, 1 ],
    4 => [ 1, 1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8, 8,
           8, 8, 8, 8, 7, 6, 5, 4, 3, 2, 1, 1, 1, 1 ],
    5 => [ 1, 1, 1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8, 8,
           8, 8, 8, 8, 7, 6, 5, 4, 3, 2, 1, 1, 1, 1, 1 ],
    6 => [ 1, 1, 1, 1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8, 8, 8, 8,
           8, 8, 8, 8, 8, 8, 7, 6, 5, 4, 3, 2, 1, 1, 1, 1, 1, 1 ],
  ];

  private array $players = [];
  private array $tricks = [];
  private int $n; // syntactic sugar pentru count($this->players)
  private int $rid, $tid; // runda și levata curentă
  private Card $trump;
  private SaveGame $save;

  function __construct(Args $args) {
    for ($i = 0; $i < $args->getNumPlayers(); $i++) {
      list($binary, $name) = $args->getPlayer($i);
      $this->players[] = new Player($binary, $name);
    }
    $this->n = count($this->players);
    if (($this->n < self::MIN_PLAYERS) || ($this->n > self::MAX_PLAYERS)) {
      Log::fatal('Jocul de whist necesită între %d și %d jucători.',
                 [ self::MIN_PLAYERS, self::MAX_PLAYERS ]);
    }
  }

  function getFirstBidder(): int {
    return $this->rid % $this->n;
  }

  function getHandSize(): int {
    return self::HAND_SIZES[$this->n][$this->rid];
  }

  function resetRound(): void {
    foreach ($this->players as $p) {
      $p->resetBid();
    }
    $this->tricks = [];
    $this->save->addRoundInfo($this->rid, $this->getFirstBidder(), $this->getHandSize());
  }

  function deal(): void {
    $deck = new Deck($this->n);
    Log::info('Am generat pachetul %s.', [ $deck ]);
    foreach ($this->players as $p) {
      $hand = $deck->dealHand($this->getHandSize());
      $p->setHand($hand);
      $this->save->addHand($hand);
    }
    $this->trump = $deck->dealTrump();
    $this->save->addTrump($this->trump);
    Log::info('Atu: %s.', [ $this->trump ]);

  }

  function getPlayerScores(): array {
    $result = [];
    foreach ($this->players as $p) {
      $result[] = $p->getScore();
    }
    return $result;
  }

  function getPlayerStreaks(): array {
    $result = [];
    foreach ($this->players as $p) {
      $result[] = $p->getStreak();
    }
    return $result;
  }

  function getPlayerBids(): array {
    $result = [];
    foreach ($this->players as $p) {
      $result[] = $p->getBid();
    }
    return $result;
  }

  function createBidState(int $p): GameState {
    $state = new GameState(
      $this->n,
      $p,
      $this->getPlayerScores(),
      $this->getPlayerStreaks(),
      $this->rid,
      $this->getHandSize(),
      $this->trump,
      $this->players[$p]->getInitialHand(),
      self::PHASE_BID,
      $this->getPlayerBids(),
    );
    return $state;
  }

  function createPlayState(int $p): GameState {
    $state = $this->createBidState($p);
    $state->phase = self::PHASE_PLAY;
    $state->trickId = $this->tid;
    $state->tricks = $this->tricks;
    return $state;
  }

  function bid(): void {
    $first = $this->getFirstBidder();
    Log::info('== Licitații începînd cu jucătorul %d.', [ $first ]);
    $handSize = $this->getHandSize();
    $sum = 0;
    for ($i = 0; $i < $this->n; $i++) {
      $p = ($i + $first) % $this->n;
      if ($i == $this->n - 1) {
        $validBids = Util::rangeExcept(0, $handSize, $handSize - $sum);
      } else {
        $validBids = range(0, $handSize);
      }

      $state = $this->createBidState($p);
      $b = $this->players[$p]->collectBid($state, $validBids);
      $sum += $b;
    }
    $this->save->addBids($this->getPlayerBids());
  }

  function playTricks(): void {
    $hs = $this->getHandSize();
    $first = $this->getFirstBidder();
    for ($this->tid = 0; $this->tid < $hs; $this->tid++) {
      Log::info('== Levata %d/%d.', [ $this->tid + 1, $hs ]);
      for ($i = 0; $i < $this->n; $i++) {
        $p = ($i + $first) % $this->n;
        $state = $this->createPlayState($p);
        if (!$i) {
          $card = $leader = $this->players[$p]->collectCard($state, Card::none(), $this->trump);
          $this->tricks[$this->tid] = new Trick();
          $t = &$this->tricks[$this->tid];
        } else {
          $card = $this->players[$p]->collectCard($state, $leader, $this->trump);
        }
        $t->cards[] = $card;
      }
      $first = $t->computeWinner($first, $this->trump);
      $this->save->addTrick($t);
      Log::info('%s cîștigă levata.', [ $this->players[$first]->name ]);
    }
  }

  function finalizeRound(): void {
    $tricksWon = array_fill(0, $this->n, 0);
    foreach ($this->tricks as $t) {
      $tricksWon[$t->winner]++;
    }

    $outcomes = [];
    $points = [];
    $bids = $this->getPlayerBids();
    $streaks = $this->getPlayerStreaks();
    for ($i = 0; $i < $this->n; $i++) {
      $madeIt = (int)($tricksWon[$i] == $bids[$i]);
      $outcomes[] = $madeIt;
      $streaks[$i] = Util::updateStreak($streaks[$i], $madeIt);
      $points[] = Util::getPoints($bids[$i], $tricksWon[$i], $streaks[$i]);
      $this->players[$i]->setStreak($streaks[$i]);
      $this->players[$i]->addScore($points[$i]);
    }

    $this->save->addOutcomes($outcomes);
    $this->save->addPoints($points);
    $this->save->addStreaks($streaks);
  }

  function finalizeGame(): void {
    Log::debug('Scoruri finale: ' . implode(' ', $this->getPlayerScores()));
  }

  function run(): void {
    $this->save = new SaveGame('game.txt');
    $this->save->addPlayers($this->players);

    $handSizes = self::HAND_SIZES[$this->n];
    for ($this->rid = 0; $this->rid < count($handSizes); $this->rid++) {
      Log::info('==== Încep runda %d/%d de mărime %d.',
                [ $this->rid + 1, count($handSizes), $this->getHandSize() ]);
      $this->resetRound();
      $this->deal();
      $this->bid();
      $this->playTricks();
      $this->finalizeRound();
    }

    $this->save->write();
    $this->finalizeGame();
  }

}
