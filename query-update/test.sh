#!/bin/bash

count=0
fail=false

while ! $fail; do
  count=$((count+1))
  echo ==== Test $count
  ./generator 500000 1000000 250000 250000 1000000 5 5 > data.in

  #for method in fenwick0 fenwick1 segtree-iter segtree-rec sqrt sqrt-pow2 sqrt-div; do
  for method in fenwick segtree-iter segtree-iter-v2 segtree-rec sqrt; do
    rm -f $method.out
    eval ./$method < data.in > $method.out
    diff -q fenwick.out $method.out || fail=true
  done
done
