#!/bin/bash

if [ "$1" == test ]; then
  for f in example_testcases/*.stdin;
  do
    ls "$f"; ./loesung < "$f" ; echo;
  done
else
  echo use test for testing through all testfiles
fi