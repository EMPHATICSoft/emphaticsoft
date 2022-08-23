#!/bin/bash
for i in {1..2}
do
  art -c readSSD_job.fcl /emph/data/users/gsdavies/artdaq/400/497/emphdata_v01.00_r497_s${i}.artdaq.root
  mv SSD.root SSD${i}.root
done
