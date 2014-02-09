#!/bin/bash
../local/bin/sox -t raw -r 44100 -e signed -b 16 -c 2 - -d
