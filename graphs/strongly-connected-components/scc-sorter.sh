#!/bin/bash

# Sort numbers within each line, then sort lines.
# https://stackoverflow.com/a/47520200/6022817
perl -lape '$_ = qq/@{[sort {$a <=> $b} @F]}/' | sort -n
