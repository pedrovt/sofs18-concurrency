#!/bin/bash

if [ -z "$1" ] 
then
  arg=2
else
  arg=$1
fi

make cleanall && make
echo "\n" | ./simulation -n $arg