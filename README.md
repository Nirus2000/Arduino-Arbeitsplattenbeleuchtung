# Arduino-Lichtsteuerung
Lichtsteuerung für Küche

Arbeitsplattenbeleuchtung Steuerung

Dieses Projekt beschreibt die Steuerung einer Arbeitsplattenbeleuchtung mit einer IR-LED, einem IR-Phototransistor und einem Relais, das eine 230V Lampe steuert.
Funktionsweise

Die IR-LED wird per Transistor wiederholt an- und ausgeschaltet. Der IR-Phototransistor erkennt reflektiertes Licht und registriert die Anwesenheit eines Gegenstands (z.B. Hand). Basierend auf der Erkennungsdauer wird das Relais gesteuert, das die Lampe ein- oder ausschaltet:

    Unter 0,2 Sekunden: Keine Aktion.
    Zwischen 0,2 und 1,0 Sekunden: Lampe schaltet ein (für 1 Stunde) oder aus.
    Zwischen 1,0 und 4,0 Sekunden: Lampe schaltet ein oder bleibt an (für 2 Stunden).
    Über 4,0 Sekunden: Keine Aktion.

Komponenten

    IR-LED
    IR-Phototransistor
    Transistor
    Relais für 230V
    Grüne LED zur Statusanzeige
    Mikrocontroller (z.B. Arduino)
    Diverse Widerstände und Verbindungskabel

Aufbau

    IR-LED und Phototransistor: Die IR-LED sendet Infrarotlicht aus, das von Gegenständen reflektiert und vom IR-Phototransistor detektiert wird.
    Transistor-Schaltung: Die IR-LED wird über einen Transistor gesteuert.
    Relais-Schaltung: Das Relais wird vom Mikrocontroller gesteuert und schaltet die 230V Lampe.
    Grüne LED: Zeigt den Status an und gibt im Fehlerfall (defekte IR-LED) den Morsecode "SOS" aus.

Programmablauf

    IR-LED Blinken: Die IR-LED wird periodisch ein- und ausgeschaltet.
    Signalverarbeitung: Der Phototransistor detektiert reflektiertes Licht. Wenn ein Gegenstand erkannt wird, leuchtet die grüne LED.
    Erkennungsdauer:
        < 0,2 Sekunden: Keine Aktion.
        0,2 - 1,0 Sekunden: Relais/Lampe ein (für 1 Stunde) oder aus.
        1,0 - 4,0 Sekunden: Relais/Lampe ein oder bleibt an (für 2 Stunden).
        > 4,0 Sekunden: Keine Aktion.
    Fehlererkennung: Wenn die IR-LED nicht mehr funktioniert, wird über die grüne LED der Morsecode "SOS" ausgegeben.

Schaltwertanpassung

Der Schaltwert IRPhotoSchalt kann geändert werden, um den Erkennungsabstand anzupassen.
Fehleranzeige

Wenn erkannt wird, dass die IR-LED nicht mehr funktioniert, zeigt die grüne LED den Morsecode "SOS" an:

    Kurz/Kurz/Kurz - Pause - Lang/Lang/Lang - Pause - Kurz/Kurz/Kurz
