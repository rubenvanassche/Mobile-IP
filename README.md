# Mobile-IP
A project for Telecom at the University Of Antwerp

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
### To Fa
```
make tf
```

### To Ha
```
make th
```

## Wat werkt er?
Al een hele hoop functionaliteit, maar daarnaast is een hoop functionaliteit nog niet altijd even juist geimplementeerd
of getest. Op deze moment worden normaal gezien advertisements, requests en replies al juist rondgestuurd. Maar een hoop dingen gebeuren nog niet : het deregistreren van node's wanneer van FA naar HA, het registereren met een onbekend homeAgent address, het opnieuw aanvragen van een registration als de lifetime dreigt te verlopen, het daadwerkelijk tunnelen van IP packets(alhoewel de infrastructuur hier zogoed als voor aanwezig is), en nog een hoop kleine zaken. Nu is het project al goed opgeschoten en moet het zeker lukken om volgende keer een mooi afgewerkte versie te hebben.

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

### MobileIPClassifier

### MobileIPDecapsulator

### MobileIPEncapsulator

### MobileIPForeignAgent

### MobileIPHomeAgent

### MobileIPNode

### MobileIPPacketTransformer

### MobileIPSoliciter
