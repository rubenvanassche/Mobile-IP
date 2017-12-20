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
De advertisement lifetime kan aangepast worden d.m.v. MobileIPAdvertiser.set_lifetime handler.

## Elementen

### MobileIPAdvertiser

### MobileIPClassifier

### MobileIPDecapsulator

### MobileIPEncapsulator

### MobileIPForeignAgent

### MobileIPHomeAgent

### MobileIPNode

### MobileIPSoliciter
