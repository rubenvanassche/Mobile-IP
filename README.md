# Mobile-IP
Een project voor Telecom op de Universiteit Antwerpen

## Compileren
```
make configure
make
```

## Runnen
```
make setup
make run
```
### Naar foreign agent
```
make tf
```

### Naar home agent
```
make th
```

## Handlers

### Home Agent

#### home_agent/mipagent
- *max_lifetime* verkrijg en pas de maximaal geaccepteerde lifetime van de home agent aan.
- *mobility_bindings* verkrijg de mobility bindings.

#### home_agent/mipadvertiser
- *set_lifetime* verkrijg en pas de  advertisement lifetime aan.
- *set_registration_lifetime* verkrijg en pas de advertisement registration lifetime aan.

### Foreign Agent

#### foreign_agent/mipadvertiser
- *max_lifetime* verkrijg en pas de maximaal geaccepteerde lifetime van de foreign agent aan.
- *visitors* verkrijg de visitors.
- *max_pending_registrations* verkrijg en pas het maximaal aantal geaccepteerde pending registrations aan.
- *request_timeout* pas aan wanneer een request time-outs na geen bericht van de HA.

#### foreign_agent/mipagent
- *set_lifetime* verkrijg en pas de  advertisement lifetime aan.
- *set_registration_lifetime* verkrijg en pas de advertisement registration lifetime aan.

### Mobile Node

#### mobile_node/mipsoliciter
- *enable_fast_moving* schakel fast moving in: wanneer een advertisement van een andere router binnenkomt zal er gemoved worden.
- *set_solicitation_interval* pas aan om de hoeveel tijd een solicitation uit te sturen, indien
 worden er geen solicitations uitgestuurd.

#### mobile_node/mipnode
- *request_timeout* pas aan wanneer een request time-outs na geen bericht van de FA.

## Elementen

### MobileIPAdvertiser
Stuurt advertisements uit om een bepaalde tijdsinterval.

### MobileIPClassifier
Classifier voor alle soorten pakketen die te maken hebben met Mobile IP, kan geconfigureerd worden zodat verschillende pakketten op verschillende poorten kunnen worden gezet.

### MobileIPDecapsulator
Decapsuleert IP-in-IP paketten door de uiterste IP header te verwijderen?

### MobileIPEncapsulator
Plaatst een pakket in een extra IP header zodat het een IP-in-IP pakket wordt. Gebeurt enkel wanneer er een mobility binding is voor de destination van het IP pakket.

### MobileIPForeignAgent
Handelt het gehele gebeuren van een FA af.

### MobileIPHomeAgent
Handelt het gehele gebeuren van een HA af.

### MobileIPNode
Handelt het gehele gebeuren van een MN af.

### MobileIPPacketTransformer
Zal pakketten die verstuurd worden vanaf de MN aanpassen, indien de MN verbonden is met een FA zal het destination ethernet address aanpassen naar dat van de FA.

### MobileIPSoliciter
Stuurt solicitations uit en ontvangt advertisements, zal ook checken of een MN gemoved is en dit eventueel doorgeven aan de MN.
