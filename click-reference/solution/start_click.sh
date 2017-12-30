#!/bin/bash

../userlevel/click glue.click -p 10000 &
./mn.bin & # listens on telnet port 10001
#../../click/userlevel/click ../../click/scripts/mn.click -p 10001 &
./ha.bin & # listens on telnet port 10002
#../../click/userlevel/click ../../click/scripts/ha.click -p 10002 &
#./fa.bin & # listens on telnet port 10003
../../click/userlevel/click ../../click/scripts/fa.click -p 10003 &
../userlevel/click cn.click -p 10004
