<?php

class Player {
  const int NO_BID = -1;

  private string $binary;
  public string $name;
  private array $initialHand, $hand; // array[Card]
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
    $this->initialHand = $hand;
    Log::default('%s a primit mîna %s.',
                 [ $this->name, Card::toString($this->hand) ]);
  }

  function getInitialHand(): array {
    return $this->initialHand;
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

  function filterBySuit(Card $card): array {
    return array_filter($this->hand, function($c) use ($card) {
      return $c->getSuit() == $card->getSuit();
    });
  }

  function playCard(Card $card): void {
    $i = 0;
    while ($this->hand[$i]->val != $card->val) {
      $i++;
    }
    array_splice($this->hand, $i, 1);
  }

  function filterHand(Card $leader, Card $trump): array {
    if ($leader->isNone()) {
      $h = $this->hand;
    } else {
      $h = $this->filterBySuit($leader);
      if (empty($h) && !$trump->isNone()) {
        $h = $this->filterBySuit($trump);
      }
      if (empty($h)) {
        $h = $this->hand;
      }
    }

    return $h;
  }

  function collectBid(GameState $state, array $choices): int {
    Log::info('Aștept licitația de la %s, variante: %s.',
              [ $this->name, implode(', ', $choices) ]);
    $choice = Interactor::interact($this->binary, $state, $choices);
    Log::info('%s licitează %d.', [ $this->name, $choice ]);
    $this->bid = $choice;
    return $choice;
  }

  function collectCard(GameState $state, Card $leader, Card $trump): Card {
    $validCards = $this->filterHand($leader, $trump);
    Log::info('Aștept cartea de la %s, leader %s, atu %s, variante: %s.',
              [ $this->name, $leader, $trump, implode(', ', $validCards) ]);
    $choices = Util::objectProperty($validCards, 'val');
    $choice = Interactor::interact($this->binary, $state, $choices);

    $card = new Card($choice);
    Log::info('%s joacă %s.', [ $this->name, $card ]);
    $this->playCard($card);
    return $card;
  }

}
