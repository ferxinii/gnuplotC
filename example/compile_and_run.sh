#!/bin/bash

rm -rf 14.mp4 15.mp4
make
./example
open 14.mp4 15.mp4
