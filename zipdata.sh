#!/bin/sh
# GZIP all files and copy them to the data folder for upload

for f in $(find ./data-working -type f -exec basename {} \;)
do
  echo "gzip -c data-working/$f > data/$f.gz"
  gzip -c data-working/$f > data/$f.gz
  # take action on each file. $f store current file name
done

