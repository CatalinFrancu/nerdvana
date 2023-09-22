#!/bin/bash

count=0
fail=false

while ! $fail; do
  count=$((count+1))
  echo ==== Test $count
  ./generator 500000 1000000000 250000 250000 1000000 5 5 > data.in

  for method in sqrt-div sqrt sqrt-pow2 segtree-rec segtree-iter fenwick0 fenwick1; do
    rm -f $method.out
    eval ./$method < data.in > $method.out
    diff -q sqrt-div.out $method.out || fail=true
  done
done
