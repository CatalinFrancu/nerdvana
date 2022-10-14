#!/bin/bash

params=(
  "01      7       3000"
  "02     11   19000000"
  "03     16   59123012"
  "04     20  993994728"

  "05    613      55123"
  "06    755      74569"
  "07    864      90843"
  "08    997      99757"

  "09  20564     572392"
  "10  46993    1230239"
  "11  60305    1793299"
  "12  82331    2891365"
  "13  91534    3939912"
  "14 100000    4938237"

  "15  75236  584739205"
  "16  80857  661987873"
  "17  85847  733912398"
  "18  93395  823712348"
  "19  97322  831234745"
  "20 100000  988271233"
)

# The generator may produce invalid test cases. Rely on the solver's assertions.
generate() {
  ./cata-gen $n $k > test/input.$test
  ./cata-logk < test/input.$test > test/output.$test
}

for triplet in "${params[@]}"; do
  read test n k <<< $triplet
  echo ==== Test $test n=$n k=$k

  until generate; do
    echo Oops, bad test, regenerating...
  done
done
