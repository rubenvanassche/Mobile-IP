# Telecom Presentatie

## Building

```
cd click
./configure --disable-linuxmodule --enable-local --enable-etherswitch
make elemlist
make -j2
```

## Setup

```
cd scripts 
sudo ./setup.sh
```

## Running
```
sudo ./start_click.sh

```

### To Foreign
```
./to_foreign.sh
```
### To Home
```
./to_home.sh
```

## Wijzigen van variabelen

### Mobile Node(port 10001)

**Soliciter**

```
# Zet Fast Moving aan of uit(indien een advertisement binnenkomt van andere agent, connecteer direct)
echo "write mobile_node/mipsoliciter.enable_fast_moving true" | telnet localhost 10001
# Pas aan om de hoeveel tijd een solicitation wordt uigezonden(0 is nooit)
echo "write mobile_node/mipsoliciter.set_solicitation_interval 10" | telnet localhost 10001
```

**Node**

```
# Pas aan wanneer een request times out
echo "write mobile_node/mipnode.request_timeout 10" | telnet localhost 10001
```

### Home Agent(port 10002)
**Advertiser**

```
# Pas lifetime van Mobile IP header aan
echo "write home_agent/mipadvertiser.set_lifetime 10" | telnet localhost 10002
# Pas lifetime van Router Advertisement aan
echo "write home_agent/mipadvertiser.set_registration_lifetime 10" | telnet localhost 10002
```

**Agent**

```
# Pas de maximaal geaccepteerde lifetime aan
echo "write home_agent/mipagent.max_lifetime 1800" | telnet localhost 10002
# Bekijk de mobility bindings
echo "read home_agent/mipagent.mobility_bindings" | telnet localhost 10002
```

### Foreign Agent(port 10003)
**Advertiser**

```
# Pas lifetime van Mobile IP header aan
echo "write foreign_agent/mipadvertiser.set_lifetime 10" | telnet localhost 10003
# Pas lifetime van Router Advertisement aan
echo "write foreign_agent/mipadvertiser.set_registration_lifetime 10" | telnet localhost 10003
```

**Agent**

```
# Pas de maximaal geaccepteerde lifetime aan
echo "write foreign_agent/mipagent.max_lifetime 1800" | telnet localhost 10003
# Pas het maximaal aantal geaccepteerde pending registrations aan
echo "write foreign_agent/mipagent.max_pending_registrations 5" | telnet localhost 10003
# Pas aan wanneer een request times out
echo "write foreign_agent/mipagent.request_timeout 5" | telnet localhost 10003
# Bekijk de visitors
echo "read foreign_agent/mipagent.visitors" | telnet localhost 10003
```
