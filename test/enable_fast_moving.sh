#!/bin/bash

echo "write mobile_node/mipsoliciter.set_solicitation_interval 3" | telnet localhost 10001
echo "write mobile_node/mipsoliciter.enable_fast_moving true" | telnet localhost 10001
