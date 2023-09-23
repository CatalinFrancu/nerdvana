#!/bin/bash

count=0
fail=false

while ! $fail; do
  count=$((count+1))
  echo ==== Test $count
  ./generator 1000 1000000 250000 250000 1000000 5 5 > data.in

  for method in fenwick1; do
    rm -f $method.out
    eval ./$method < data.in > $method.out
    diff -q fenwick1.out $method.out || fail=true
  done
done
