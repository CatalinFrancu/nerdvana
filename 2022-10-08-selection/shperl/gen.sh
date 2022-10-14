#!/bin/bash

echo generating tests...

./cata-gen  10000   5000   5000 20 20 > test/input.01
./cata-gen  10000   5000   5000  1  1 > test/input.02

./cata-gen  50000  25000  25000 20 20 > test/input.03
./cata-gen  50000  25000  25000  1  1 > test/input.04

./cata-gen  80000  40000  40000 20 20 > test/input.05
./cata-gen  80000  40000  40000  1  1 > test/input.06

./cata-gen 100000  50000  50000 20 20 > test/input.07
./cata-gen 100000  50000  50000  1  1 > test/input.08

./cata-gen 120000  60000  60000 20 20 > test/input.09
./cata-gen 120000  60000  60000  1  1 > test/input.10

./cata-gen 150000  75000  75000 20 20 > test/input.11
./cata-gen 150000  75000  75000  1  1 > test/input.12

./cata-gen 200000 100000 100000 20 20 > test/input.13
./cata-gen 200000 100000 100000  1  1 > test/input.14

./cata-gen 250000 125000 125000 20 20 > test/input.15
./cata-gen 250000 125000 125000  1  1 > test/input.16

./cata-gen 280000 140000 140000 20 20 > test/input.17
./cata-gen 280000 140000 140000  1  1 > test/input.18

./cata-gen 300000 150000 150000 20 20 > test/input.19
./cata-gen 300000 150000 150000  1  1 > test/input.20

echo creating outputs...

for i in test/input.*; do
  j="${i/input/output}"
  ./cata-segtree < $i > $j
done
