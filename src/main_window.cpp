#include "main_window.hpp"
#include <gdk/gdkkeysyms.h>

MainWindow::MainWindow()
{
    // Fullscreen-Modus aktivieren
    fullscreen();

    // Haupt-Box zentriert ausrichten
    m_main_box.set_halign(Gtk::Align::CENTER);
    m_main_box.set_valign(Gtk::Align::CENTER);
    m_main_box.set_spacing(24);
    m_main_box.set_margin_top(20);
    m_main_box.set_margin_bottom(20);
    m_main_box.set_margin_start(30);
    m_main_box.set_margin_end(30);

    // Titel
    m_title_label.set_markup("<span size='xx-large' weight='bold'>Kassensystem</span>");
    m_main_box.append(m_title_label);

    // Raster (Grid) für POS-Buttons konfigurieren (3 Spalten x 2 Zeilen)
    m_grid.set_row_spacing(16);
    m_grid.set_column_spacing(16);

    int col = 0;
    int row = 0;
    for (auto* btn : m_articles) {
        m_grid.attach(*btn, col, row, 1, 1);

        // Klick auf Artikel aktualisiert die Gesamtübersicht
        btn->signal_clicked().connect([this]() {
            update_total_display();
        });

        // Langer Druck (>3s) setzt Artikel auf 0 zurück und aktualisiert die Gesamtübersicht
        btn->signal_long_pressed().connect([this]() {
            update_total_display();
        });

        col++;
        if (col >= 3) {
            col = 0;
            row++;
        }
    }
    m_main_box.append(m_grid);

    // Untere Leiste: Summenanzeige und Reset-Button
    m_bottom_box.set_spacing(20);
    m_bottom_box.set_halign(Gtk::Align::CENTER);

    update_total_display();
    m_bottom_box.append(m_summary_label);

    m_reset_button.set_size_request(160, 45);
    m_reset_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::reset_all));
    m_bottom_box.append(m_reset_button);

    m_main_box.append(m_bottom_box);

    set_child(m_main_box);

    // Tastatur-Controller hinzufügen (Escape schließt das Vollbildfenster)
    auto key_controller = Gtk::EventControllerKey::create();
    key_controller->signal_key_pressed().connect(
        [this](guint keyval, guint /*keycode*/, Gdk::ModifierType /*state*/) {
            if (keyval == GDK_KEY_Escape) {
                close();
                return true;
            }
            return false;
        },
        false
    );
    add_controller(key_controller);

    m_btn_kaffee.set_visible(false);
}

void MainWindow::update_total_display()
{
    double total = 0.0;
    int total_items = 0;

    for (const auto* btn : m_articles) {
        total += btn->get_totalprice();
        total_items += btn->get_amount();
    }

    const std::string text = "<span size='large'>Gesamtsumme: <b>" +
                             ArticleButton::format_currency(total).raw() +
                             "</b> (" + std::to_string(total_items) + " Artikel)</span>";
    m_summary_label.set_markup(text);
}

void MainWindow::reset_all()
{
    for (auto* btn : m_articles) {
        btn->reset_amount();
    }
    update_total_display();
}
