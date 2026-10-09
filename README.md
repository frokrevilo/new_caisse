# Caisse (C++ / gtkmm-4.0)

Einfache dialogbasierte GTK-Applikation in modernem C++ (C++20) mit `gtkmm-4.0`.

## Voraussetzungen

Für die Kompilierung wird das Entwicklerpaket für `gtkmm-4.0` benötigt:

```bash
sudo apt update
sudo apt install -y libgtkmm-4.0-dev build-essential
```

## Kompilieren & Ausführen

### Mit Make:

```bash
# Bauen
make

# Ausführen
make run
# oder
./caisse
```

### Mit Meson / Ninja:

```bash
meson setup build
meson compile -C build
./build/caisse
```

## Eigenschaften

- Startet automatisch als **Fullscreen**-Kassenansicht.
- Bietet eine wiederverwendbare Button-Klasse [`ArticleButton`](file:///home/oliver/Development/c++/gtk/caisse/src/article_button.hpp), abgeleitet von `Gtk::Button`:
  - **article**: Zentrale Beschriftung (z. B. "Kaffee", "Tee").
  - **color**: Frei wählbare Farbe mit automatischer Helligkeits-/Kontrastanpassung für die Beschriftung.
  - **price**: Artikelpreis rechts oben im Button platziert (`get_price()` / `set_price()`).
  - **amount**: Klickzähler mit optischem Badge oben links (`get_amount()`, `reset_amount()`).
  - **totalprice**: Berechneter Gesamtpreis (`amount * price`, `get_totalprice()`).
  - **Langer Tastendruck (>3s)**: Setzt die Anzahl dieses Artikels direkt auf `0` zurück.
  - **Taktiles 3D-Design**: Schlagschatten mit realistischem Absenken (`transform: translateY(3px)`) beim Drücken.
- Zeigt eine Kassen-Übersicht mit Gesamtsummenberechnung und Reset-Funktion.
- Lässt sich jederzeit über die **Escape**-Taste (oder Alt+F4) beenden.
