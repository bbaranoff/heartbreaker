#!/bin/bash
cd /icebreaker/libosmocore
git checkout 1.1.0
make
make install
ldconfig
