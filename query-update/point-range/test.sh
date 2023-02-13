#!/bin/bash

count=0
fail=false

while ! $fail; do
  count=$((count+1))
  echo ==== Test $count
  ./generator 500000 1000000000 100000 400000 1000000 5 5 > data.in

  for method in sqrt segtree segtree2 fenwick; do
    rm -f $method.out
    eval ./$method < data.in > $method.out
    diff sqrt.out $method.out || fail=true
  done
done
