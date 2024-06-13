#!/bin/bash

readonly NUM_GAMES=3
readonly CMD="php arbiter.php
  -r
  -b /calea/spre/client1 -n nume1
  -b /calea/spre/client2 -n nume2
  -b /calea/spre/client3 -n nume3
  -b /calea/spre/client4 -n nume4
"

declare -A totals

function process_output() {
  while IFS=$' \t\r\n' read -r line;do
    name=${line%% *}
    score=${line#* }
    score=$(echo "$score" | tr -d '')
    totals[$name]=$((totals[$name]+$score))
  done <<< "$@"
}

function print_totals() {
  echo ==== TOTAL
  for name in ${!totals[@]}; do
    echo $name ${totals[${name}]}
  done | sort -rn -k2
}

function main() {
  echo "Comanda: $CMD"
  for i in `seq 1 $NUM_GAMES`; do
    echo ==== Rulez partida $i din $NUM_GAMES
    # Șterge culorile ANSI, păstrează doar caracterele printabile
    OUTPUT=$($CMD | grep "Scor final:" | sed -e 's/\x1b\[[0-9;]*m//g' | cut -d ' ' -f 3,4)
    echo "$OUTPUT"
    process_output "$OUTPUT"
    mv game.txt game${i}.txt
  done

  print_totals
}

main "$@"
