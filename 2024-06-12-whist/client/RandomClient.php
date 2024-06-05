#!/usr/bin/php
<?php

const INPUT_FILE = 'input.txt';
const OUTPUT_FILE = 'output.txt';
const PHASE_BID = 0;
const PHASE_PLAY = 1;

main();

function readInput(): array {
  $lines = file(INPUT_FILE);
  $state = [];

  foreach ($lines as $l) {
    list($key, $value) = explode(' ', $l, 2);
    $value = trim($value);
    if (is_numeric($value)) {
      $state[$key] = $value;
    } else {
      $state[$key] = explode(' ', $value);
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

function writeOutput(int $choice) {
  file_put_contents(OUTPUT_FILE, $choice . "\n");
}

function main() {
  $state = readInput();

  if ($state['phase'] == PHASE_BID) {
    $choice = bid($state);
  }

  writeOutput($choice);
}
