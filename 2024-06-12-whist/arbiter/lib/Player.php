<?php

class Player {
  const int NO_BID = -1;

  private string $binary;
  private string $name;
  private array $hand; // array[Card]
  private int $score;
  private int $streak;
  private int $bid;

  function __construct(string $binary, string $name) {
    $this->binary = realpath($binary);
    $this->name = $name;
    $this->score = 0;
    $this->streak = 0;
    $this->resetBid();
    Log::info('Am adăugat jucătorul %s cu binarul %s.', [ $name, $binary ]);
  }

  function setHand(array $hand): void {
    $this->hand = $hand;
    Log::default('%s a primit mîna %s.',
                 [ $this->name, Card::toString($this->hand) ]);
  }

  function getHand(): array {
    return $this->hand;
  }

  function getScore(): int {
    return $this->score;
  }

  function getStreak(): int {
    return $this->streak;
  }

  function getBid(): int {
    return $this->bid;
  }

  function resetBid(): void {
    $this->bid = self::NO_BID;
  }

  function collectBid(GameState $state, array $choices): int {
    Log::info('Aștept licitația de la %s, variante: %s.',
              [ $this->name, implode(', ', $choices) ]);
    $choice = Interactor::interact($this->binary, $state, $choices);
    Log::info('%s licitează %d.', [ $this->name, $choice ]);
    $this->bid = $choice;
    return $choice;
  }

}
