#!/usr/bin/php
<?php

const INPUT_FILE = 'input.txt';
const OUTPUT_FILE = 'output.txt';
const PHASE_BID = 0;
const PHASE_PLAY = 1;
const NUM_RANKS = 13;
const NONE = -1;

const ARRAY_PARAMS = [ 'scores', 'streaks', 'initial_hand', 'bids' ];

main();

function readInput(): array {
  $lines = file(INPUT_FILE);
  $state = [
    'trick' => [],
  ];

  foreach ($lines as $l) {
    list($key, $value) = explode(' ', $l, 2);
    $value = trim($value);

    if ($key == 'trick') {
      $parts = explode(' ', $value);
      array_shift($parts); // ignoră primul argument, adică lungimea
      $state['trick'][] = $parts;
    } else if (in_array($key, ARRAY_PARAMS)) {
      $state[$key] = explode(' ', $value);
    } else {
      $state[$key] = $value;
    }
  }

  return $state;
}

function bid(array $state): int {
  $forbidden = -1;
  if ($state['player_order'] == $state['num_players'] - 1) {
    // Adaugă 1 pentru unicul -1 din bids.
    $forbidden = $state['hand_size'] - (array_sum($state['bids']) + 1);
  }

  do {
    $choice = rand(0, $state['hand_size']);
  } while ($choice == $forbidden);

  return $choice;
}

function filterPlayed(array $state): array {
  $h = $state['initial_hand'];
  foreach ($state['trick'] as $t) {
    $h = array_diff($h, $t);
  }
  return $h;
}

function filterBySuit(array $h, int $card): array {
  return array_filter($h, function($c) use ($card) {
    return intdiv($c, NUM_RANKS) == intdiv($card, NUM_RANKS);
  });
}

function filterLegal(array $state, array $h): array {
  $tid = $state['trick_id'];
  if (!isset($state['trick'][$tid])) {
    return $h;
  }

  $leader = $state['trick'][$tid][0];
  $trump = $state['trump'];

  $f = filterBySuit($h, $leader);
  if (empty($f) && ($trump != NONE)) {
    $f = filterBySuit($h, $trump);
  }
  if (empty($f)) {
    $f = $h;
  }
  return $f;
}

function play(array $state): int {
  $h = filterPlayed($state);
  $h = filterLegal($state, $h);
  return $h[array_rand($h)];
}

function writeOutput(int $choice) {
  file_put_contents(OUTPUT_FILE, $choice . "\n");
}

function main() {
  $state = readInput();

  if ($state['phase'] == PHASE_BID) {
    $choice = bid($state);
  } else {
    $choice = play($state);
  }

  writeOutput($choice);
}
