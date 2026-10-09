#include "scale_helper.hpp"
#include <giomm/listmodel.h>
#include <iostream>
#include <cstdlib>
#include <sstream>

ScaleHelper& ScaleHelper::instance()
{
    static ScaleHelper s_instance;
    return s_instance;
}

ScaleHelper::ScaleHelper()
{
    set_resolution(1920, 1080);
}

void ScaleHelper::init_from_display(const Glib::RefPtr<Gdk::Display>& display)
{
    // 1. Prüfen, ob über Umgebungsvariable CAISSE_SCALE vorgegeben
    const char* env_scale = std::getenv("CAISSE_SCALE");
    if (env_scale) {
        try {
            double factor = std::stod(env_scale);
            if (factor > 0.1) {
                set_factor(factor);
                std::cout << "[ScaleHelper] CAISSE_SCALE aktiv: Faktor = " << m_factor << std::endl;
                m_initialized = true;
                return;
            }
        } catch (...) {
            // Ignorieren und mit Erkennung fortfahren
        }
    }

    // 2. Prüfen, ob über Umgebungsvariable CAISSE_RES vorgegeben (z. B. "1366x768")
    const char* env_res = std::getenv("CAISSE_RES");
    if (env_res) {
        std::string s(env_res);
        auto pos = s.find('x');
        if (pos != std::string::npos) {
            try {
                int w = std::stoi(s.substr(0, pos));
                int h = std::stoi(s.substr(pos + 1));
                if (w > 0 && h > 0) {
                    set_resolution(w, h);
                    std::cout << "[ScaleHelper] CAISSE_RES aktiv: " << w << "x" << h
                              << " -> Faktor = " << m_factor << std::endl;
                    m_initialized = true;
                    return;
                }
            } catch (...) {
                // Ignorieren
            }
        }
    }

    // 3. Aus dem GDK Display den Monitor abfragen
    if (display) {
        try {
            auto monitors = display->get_monitors();
            if (monitors && monitors->get_n_items() > 0) {
                auto monitor = std::dynamic_pointer_cast<Gdk::Monitor>(monitors->get_object(0));
                if (monitor) {
                    Gdk::Rectangle rect;
                    monitor->get_geometry(rect);
                    if (rect.get_width() > 0 && rect.get_height() > 0) {
                        set_resolution(rect.get_width(), rect.get_height());
                        std::cout << "[ScaleHelper] Monitor erkannt: "
                                  << rect.get_width() << "x" << rect.get_height()
                                  << " -> Skalierungsfaktor: " << m_factor << std::endl;
                        m_initialized = true;
                        return;
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "[ScaleHelper] Fehler bei Monitorabfrage: " << e.what() << std::endl;
        } catch (...) {
            // Unbekannter Fehler
        }
    }

    // 4. Fallback auf Full-HD
    if (!m_initialized) {
        set_resolution(1920, 1080);
    }
}

void ScaleHelper::set_resolution(int width, int height)
{
    if (width <= 0 || height <= 0) {
        width = 1920;
        height = 1080;
    }
    m_width = width;
    m_height = height;

    // Basierend auf Referenz 1920x1080 Full-HD
    const double scale_w = static_cast<double>(width) / 1920.0;
    const double scale_h = static_cast<double>(height) / 1080.0;

    // Minimum verwenden, damit bei abweichenden Formaten (z. B. 4:3 oder 16:10)
    // weder horizontal noch vertikal Elemente überlaufen
    m_factor = std::min(scale_w, scale_h);

    // Auf einen sinnvollen Bereich begrenzen (z. B. 0.45 bis 3.0)
    m_factor = std::clamp(m_factor, 0.45, 3.0);
}

void ScaleHelper::set_factor(double factor)
{
    m_factor = std::clamp(factor, 0.45, 3.0);
    m_width = static_cast<int>(std::round(1920.0 * m_factor));
    m_height = static_cast<int>(std::round(1080.0 * m_factor));
}
