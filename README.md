# Examensarbete - Jesper Morais & David Stenman👍

## IoT Controller - **Beskrivning**

Projektet syftar till att utveckla en trådlös, batteridriven kontroller baserad på ESP32 med en integrerad gyrosensor (t.ex. MPU6050).

Denna kontroller är utformad som en flexibel plattform för att styra och interagera med flera olika applikationer. 

Med hjälp av rörelsedata från gyroskopet och accelerometern kan kontrollern samla in information om vinklar, rörelser och rotation, och överföra dessa data i realtid till en Go-server via Wi-Fi.

### [ ESP32 BOOT ] 
   → Starta WiFi (Task 1)
   → Anslut till servern (UDP)
   → Skicka anslutningspaket

### [ GO SERVER ]
   → Ta emot ny spelare
   → Skapa boll på skärmen
   → Skicka tillbaka Player ID

### [ ESP32 ]
   → Starta sensor-task (Task 2)
   → Läs gyroskopdata
   → Skicka sensor-data till servern (Task 3)

### [ GO SERVER ]
   → Ta emot gyro-data
   → Konvertera gyro → position
   → Skicka uppdaterad position till alla spelare

### [ WEBSOCKET / GAME ENGINE ]
   → Rendera boll på skärmen i realtid

### [ EVENTUELL INTERAKTION ]
   → Om ESP32 skakas: "Jump!"
   → Om WiFi tappar kontakt: "Pausa"
   → Om spelet avslutas: "Stäng anslutning"
