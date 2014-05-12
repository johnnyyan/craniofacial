#!/bin/bash

EXE=~/qual/test/AutoLandmarks-build/AutoLandmarks
DIR=${1%/}

for FILE in `ls $DIR/*.ply`; do
  "$EXE" "$FILE"
done
