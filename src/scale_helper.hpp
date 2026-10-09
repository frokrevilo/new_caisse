#pragma once

#include <gdkmm/display.h>
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
    /// Liefert die globale Singleton-Instanz.
    static ScaleHelper& instance();

    // Singleton: Kopieren und Zuweisen verhindern
    ScaleHelper(const ScaleHelper&) = delete;
    ScaleHelper& operator=(const ScaleHelper&) = delete;

    /// Initialisiert die Skalierung anhand des Standard-Monitors des angegebenen Displays.
    void init_from_display(const Glib::RefPtr<Gdk::Display>& display = Gdk::Display::get_default());

    /// Manuelle Festlegung der Zielauflösung (z.B. bei Fensteränderungen oder Tests).
    void set_resolution(int width, int height);

    /// Manuelle Vorgabe eines Skalierungsfaktors (z. B. 1.0 = Full HD, 0.71 = 768p, 2.0 = 4K).
    void set_factor(double factor);

    /// Aktuell erkannte/gesetzte Bildschirmbreite in Pixeln.
    [[nodiscard]] int screen_width() const { return m_width; }

    /// Aktuell erkannte/gesetzte Bildschirmhöhe in Pixeln.
    [[nodiscard]] int screen_height() const { return m_height; }

    /// Skalierungsfaktor bezogen auf 1920x1080 (z. B. 1.0 bei Full-HD, 0.71 bei 768p, 2.0 bei 4K).
    [[nodiscard]] double factor() const { return m_factor; }

    /**
     * @brief Skaliert einen Basis-Pixelwert proportional zur aktuellen Bildschirmauflösung.
     * @param base_px Pixelwert bei 1920x1080 Referenzauflösung.
     * @return Skalierter Pixelwert (mindestens 1 px bei base_px > 0).
     */
    [[nodiscard]] int px(int base_px) const {
        if (base_px <= 0) return base_px;
        return std::max(1, static_cast<int>(std::round(base_px * m_factor)));
    }

    /**
     * @brief Skaliert eine Pango-Punktgröße (pt) proportional zur Bildschirmauflösung.
     * @param base_pt Punktgröße bei 1920x1080 Referenzauflösung.
     * @param min_pt Untergrenze zur Gewährleistung der Mindestlesbarkeit (Standard: 6pt).
     * @return Skalierte Punktgröße in pt.
     */
    [[nodiscard]] int pt(int base_pt, int min_pt = 6) const {
        return std::max(min_pt, static_cast<int>(std::round(base_pt * m_factor)));
    }

    /**
     * @brief Gibt die skalierte Punktgröße als Pango-Markup-String zurück (z. B. "18pt").
     */
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
