#!/bin/bash
ipcrm -a
sed -i "s/MODE 2/MODE 1/g" control.h
make
./sr &!
./sw

sed -i "s/MODE 1/MODE 2/g" control.h
make
./sr &!
./sw

python get_latency.py
