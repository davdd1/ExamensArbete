# Examensarbete – Jesper Morais & David Stenman 👍

## IoT Controller – **Beskrivning**

Detta projekt syftar till att utveckla en trådlös, batteridriven kontroller baserad på ESP32, utrustad med en integrerad gyrosensor (t.ex. MPU6050).

Kontrollern är designad som en flexibel plattform för att styra och interagera med olika applikationer, exempelvis spel, robotik eller smarta hem-system.

Med hjälp av rörelsedata från gyroskopet och accelerometern kan kontrollern samla in information om vinklar, rörelser och rotation. Dessa data skickas i realtid via Wi-Fi till en Go-server, där de kan bearbetas vidare eller användas för att styra andra enheter.

### Funktioner

- **ESP32-baserad**: Kraftfull mikrokontroller med inbyggd Wi-Fi och Bluetooth.
- **MPU6050**: 6-axlig rörelsesensor (gyroskop + accelerometer) för exakt rörelsedetektering.
- **Trådlös kommunikation**: Data skickas via Wi-Fi till en central server.
- **Realtidsöverföring**: Snabb och pålitlig överföring av sensordata.
- **Flexibel plattform**: Kan enkelt anpassas för olika användningsområden och applikationer.
- **LED-feedback**: Integrerad LED-strip för visuell återkoppling och statusindikering.

### Systemöversikt

1. **Sensorinsamling**: ESP32 läser kontinuerligt data från MPU6050.
2. **Databehandling**: Rådata kan filtreras och omvandlas till användbara värden (t.ex. Euler-vinklar).
3. **Kommunikation**: Data skickas via UDP till en Go-server i samma nätverk.
4. **Serverhantering**: Go-servern tar emot, tolkar och vidarebefordrar data till önskad applikation.

### Kom igång

#### Förutsättningar

- ESP32-utvecklingskort
- MPU6050-modul
- Adress och port till Go-servern
- ESP-IDF (rekommenderad version 5.x)
- Go installerat på servern

#### Bygga och flasha firmware

1. Klona detta repository.
2. Konfigurera Wi-Fi-inställningar och serveradress.
3. Bygg och flasha projektet med:
   ```sh
   idf.py build
   idf.py -p [PORT] flash monitor
   ```