#!/bin/bash

if [ "$1" == test ]; then
  > result.txt # clear result.txt

  # add all results of testfiles to result.txt
  for f in example_testcases/*.stdin;
  do
    echo "running $f"; (./loesung < "$f") >> result.txt ;
  done

  # check for differences
  echo check:
  diff result.txt realRes.txt
  echo check complete.

elif [ "$1" == check ]; then
  # check all testfiles for memory leaks
  for f in example_testcases/*.stdin;
  do
    echo "running $f"; ( (valgrind ./loesung) > /dev/null) < "$f" ;
  done
else
  echo use \'test\' for testing through all testfiles
fi