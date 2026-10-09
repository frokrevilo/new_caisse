#pragma once

#include <gdkmm/display.h>
#include <gdkmm/monitor.h>
#include <gdkmm/rectangle.h>
#include <string>
#include <algorithm>
#include <cmath>

/**
 * @brief Verwaltet die auflösungsabhängige Skalierung der gesamten Kassen-UI.
 *
 * Berechnet Skalierungsfaktoren ausgehend von einer Referenzauflösung (1920x1080 Full-HD).
 * Unterstützt proportionale Skalierung von:
 * - Pixelwerten (px) für Schaltflächenhöhen, Abstände, Polsterungen und Radien
 * - Pango-Schriftgraden (pt) für Labels und Markup
 * - Fenster- und Dialog-Dimensionen
 *
 * Kann optional über Umgebungsvariablen gesteuert werden:
 * - CAISSE_SCALE (z. B. "1.25" oder "0.75")
 * - CAISSE_RES   (z. B. "1366x768" oder "3840x2160")
 */
class ScaleHelper {
public:
    static ScaleHelper& instance();

    /// Initialisiert die Skalierung anhand des Standard-Monitors des angegebenen Displays.
    void init_from_display(const Glib::RefPtr<Gdk::Display>& display = Gdk::Display::get_default());

    /// Manuelle Festlegung der Zielauflösung.
    void set_resolution(int width, int height);

    /// Manuelle Vorgabe eines Skalierungsfaktors (z. B. 1.0 = Full HD).
    void set_factor(double factor);

    /// Aktuell erkannte/gesetzte Bildschirmbreite in Pixeln.
    [[nodiscard]] int screen_width() const { return m_width; }

    /// Aktuell erkannte/gesetzte Bildschirmhöhe in Pixeln.
    [[nodiscard]] int screen_height() const { return m_height; }

    /// Skalierungsfaktor bezogen auf 1920x1080 (z. B. 1.0 bei Full-HD, 0.71 bei 768p, 2.0 bei 4K).
    [[nodiscard]] double factor() const { return m_factor; }

    /// Skaliert einen Pixelwert proportional zur Auflösung (mindestens 1 px bei base_px > 0).
    [[nodiscard]] int px(int base_px) const {
        if (base_px <= 0) return base_px;
        return std::max(1, static_cast<int>(std::round(base_px * m_factor)));
    }

    /// Skaliert eine Pango-Punktgröße (pt), optional mit Untergrenze min_pt.
    [[nodiscard]] int pt(int base_pt, int min_pt = 6) const {
        return std::max(min_pt, static_cast<int>(std::round(base_pt * m_factor)));
    }

    /// Gibt die skalierte Punktgröße als Pango-Markup-String zurück (z. B. "18pt").
    [[nodiscard]] std::string pt_str(int base_pt, int min_pt = 6) const {
        return std::to_string(pt(base_pt, min_pt)) + "pt";
    }

private:
    ScaleHelper();

    int m_width{1920};
    int m_height{1080};
    double m_factor{1.0};
    bool m_initialized{false};
};
