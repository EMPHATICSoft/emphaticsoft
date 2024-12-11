#!/bin/bash

FIRST_RUN=2293
timestamp=$(date +%N | cut -c 1-8 - | sed -E 's/0*(.*)/\1/g') #Last 8 digits of nanoseconds because GEANT only supports seeds up to 9x10^8
RUN=$((PROCESS + FIRST_RUN))
SEED=$((PROCESS + timestamp))

sed "s/@@RUN@@/$RUN/g" < $1 > withRun.fcl
sed "s/@@SEED@@/$SEED/g" < withRun.fcl
