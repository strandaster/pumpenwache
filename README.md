# Pumpenwache
Ziel des Projektes war, die Pausenzeiten die Pumpe der Drainage, die um unser Haus liegt, zu überwachen. Anhand der Pausenzeiten kann man abschätzen, ob viel oder wenig Wasser am Haus steht. Außerdem ist eine Funktionskontrolle gegeben. Um die Umgebungswerte des Kellerraumes zu überwachen, wurde noch ein Temperatur- und Feuchtigkeitssensor verbaut.

## Technische Umsetzung
### benötigte Bauteile
* Stromsensor ACS712 (passend zur erwarteten Stromstärke)
* ESP8266 (in diesem Projekt wurde ein Wemos D1 mini verwendet)
* BME280 (für die Messung der Umgebungswerte)
* Kleinteile um die Schaltung aufzubauen

### Schaltplan
![Schaltplan](/doc/PumpenwacheV2_Schaltplan.jpg)

![Steckbrett-Ansicht](/doc/PumpenwacheV2_Steckplatine.jpg)

### Software
**verwendete Bibliotheken:**
  * ACS712-Arduino (<https://github.com/strandaster/ACS712-arduino.git>)
  * BME280 (ID:901) (<https://github.com/finitespace/BME280.git>)
  * Telnetspy (<https://github.com/strandaster/telnetspy.git>)

Die Bibliotheken ACS712-Arduino und TelnetSpy wurden angepasst, damit sie mit der aktuellen Arduino Version funktionieren.

**Ablaufbeschreibung:**
Die Umgebungswerte wie Temperatur und Luftfeuchtigkeit werden alle 15 Minuten abgefragt und durch den Aufruf eines PHP-Skriptes auf eine entfernte mySQL-Datenbank gespeichert.
Alle 15 Sekunden wird geprüft, ob ein Stromfluss am Sensor ACS712 detektiert werden kann. Übersteigt der Wert einen Schwellwert, wird die Pumpe als aktiv gekennzeichnet. Fällt der Wert wieder (die Pumpe hat den Pumpensumpf leergepumpt), wird ein PHP-Skript aufgerufen, um die letzte Pumpenaktivität zu melden.

Für Debug-Zwecke kann sich mittels Telnet (Port 23) mit dem Mikrocontroller verbunden werden. 

Um den Quellcode zu aktualisieren "drahtlos" zu aktualisieren, wurde ArduinoOTA implementiert. 

### Auswertung
In der mySQL Datenbank werden folgende Objekte benötigt:
* Tabelle **_dataV2_**
  * **_Time_** timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
  * **_Amps_** float NOT NULL
* Trigger auf _dataV2_ um einen Eintrag in der Tabelle _diff_ anzulegen
* Tabelle **_diff_**
  * **_Time_** timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
  * **_diff_** int(11) DEFAULT NULL
* Tabelle **_Umwelt_**
  * **_Time_** timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
  * **_Temperatur_** float DEFAULT NULL
  * **_Feuchtigkeit_** float DEFAULT NULL
  * **_Luftdruck_** float DEFAULT NULL

Für die grafische Auswertung wird [Google Charts](https://developers.google.com/chart) genutzt. 
Die Pumpenaktivität wird mittels _pumpe.php_ und die Umgebungswerte mittels _umwelt.php_ visualisiert.

Für die Nutzung der Werte im [Home Assistant](https://www.home-assistant.io) übergibt das PHP Skript _umweltsensor.php_ die Werte als JSON Array. 