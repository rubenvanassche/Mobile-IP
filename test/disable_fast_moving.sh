#!/bin/bash

echo "write mobile_node/mipsoliciter.set_solicitation_interval 1800" | telnet localhost 10001
echo "write mobile_node/mipsoliciter.enable_fast_moving false" | telnet localhost 10001
