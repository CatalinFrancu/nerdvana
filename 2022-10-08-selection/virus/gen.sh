#!/bin/bash

./cata-gen   20     3    97 > test/input.01
./cata-gen   20     5    95 > test/input.02
./cata-gen   20    10    90 > test/input.03

./cata-gen   60    20   780 > test/input.04
./cata-gen   60    50   750 > test/input.05
./cata-gen   60    70   730 > test/input.06

./cata-gen  100    50   950 > test/input.07
./cata-gen  100   130   870 > test/input.08
./cata-gen  100   200   800 > test/input.09

./cata-gen  800   500 39500 > test/input.10
./cata-gen  800  3000 37000 > test/input.11
./cata-gen  800 10000 30000 > test/input.12

./cata-gen 1000   600 49400 > test/input.13
./cata-gen 1000  7000 43000 > test/input.14
./cata-gen 1000 15000 35000 > test/input.15

./cata-gen 1200   700 59300 > test/input.16
./cata-gen 1200  8000 52000 > test/input.17
./cata-gen 1200 24000 36000 > test/input.18

./cata-gen 1400   800 69200 > test/input.19
./cata-gen 1400 10000 60000 > test/input.20
./cata-gen 1400 30000 40000 > test/input.21

./cata-gen 1600  1000 79000 > test/input.22
./cata-gen 1600 12000 68000 > test/input.23
./cata-gen 1600 40000 40000 > test/input.24

./cata-gen 1800  1000 89000 > test/input.25
./cata-gen 1800 15000 75000 > test/input.26
./cata-gen 1800 45000 45000 > test/input.27

./cata-gen 2000  1000 99000 > test/input.28
./cata-gen 2000 20000 80000 > test/input.29
./cata-gen 2000 50000 50000 > test/input.30
