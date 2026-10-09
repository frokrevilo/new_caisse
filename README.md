# Caisse (C++20 / gtkmm-4.0 POS Kassensystem)

Ein modernes, touch-optimiertes Point-of-Sale (POS) Kassensystem in C++20 mit `gtkmm-4.0`.

## Features

- **4×5 Artikel-Matrix (linke Seite — 3/4 des Platzes):**
  - Bis zu 20 konfigurierbare Artikel-Buttons im flexiblen Raster.
  - Vollständig anpassbar über [`articles.yaml`](file:///home/oliver/Development/c++/gtk/caisse/articles.yaml).
  - Unterstützt `artikelname`, `artikelpreis`, `farbe`, `sichtbarkeit` und `position (x, y)`.
  - Schlagschatten mit taktilem 3D-Klick-Effekt (`box-shadow`, `translateY(3px)`).
  - Klickzähler-Badge auf jedem Button.
  - Langer Tastendruck (>3s) setzt den Zähler für den jeweiligen Artikel auf `0` zurück.
- **Kassenübersicht & Abrechnung (rechte Seite — 1/4 des Platzes):**
  - Große, gut lesbare Schriftarten für Touch- und Kassendisplays.
  - **Gesamtpreis:** Summiert automatisch alle ausgewählten Artikel in extragroßer Schrift.
  - **Erhaltener Betrag:** Interaktive Schaltfläche. Ein Klick öffnet den Eingabedialog.
  - **Rückgeld:** Live-Berechnung des Rückgelds (grün hervorgehoben bei Überzahlung, rot bei Fehlbetrag).
  - **Bezahlt:** Großer Aktionsknopf, wechselt dynamisch auf Grün, sobald der erhaltene Betrag ausreicht.
  - **Abbruch:** Roter Aktionsknopf (`#dc2626`), bricht den aktuellen Vorgang ab und leert alle Zähler.
- **Touch-Ziffernblock (Dialog für erhaltenen Betrag):**
  - Große, gut lesbare Knöpfe für Touch-Bedienung ohne überfrachtende Schnellwahltasten.
  - Layout:
    ```text
    7  8  9
    4  5  6
    1  2  3
    0  ,  CLR
    ```
  - Tastaturunterstützung (Ziffern, Komma, Backspace, Enter, Escape).
- **Kompakt & Performant:**
  - Kompiliert als hochoptimiertes, schlankes Binary (~132 KB).

## Voraussetzungen

Entwicklerpakete für `gtkmm-4.0` installieren:

```bash
sudo apt update
sudo apt install -y libgtkmm-4.0-dev build-essential
```

## Kompilieren & Starten

```bash
# Bauen
make

# Starten
make run
# oder
./caisse
```

- **Escape:** Beendet das Programm.
- **F11:** Schaltet zwischen Vollbild und Fenstermodus um.

## Konfiguration (`articles.yaml`)

Artikel können direkt in `articles.yaml` bearbeitet oder ergänzt werden:

```yaml
articles:
  - artikelname: "Kaffee"
    artikelpreis: 2.50
    farbe: "#8B5A2B"
    sichtbarkeit: true
    position: [0, 1]
```
