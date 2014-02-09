#!/bin/bash
curl $1 | ../local/bin/lame -t --mp3input --decode - -
