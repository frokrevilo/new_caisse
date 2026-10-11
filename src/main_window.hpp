#pragma once

#include "article_button.hpp"
#include "cash_input_dialog.hpp"
#include <gtkmm/window.h>
#include <gtkmm/grid.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/cssprovider.h>
#include <vector>
#include <memory>

/**
 * @brief Hauptfenster des Kassensystems (caisse).
 *
 * Layout:
 * - Links (3/4 des Platzes): 4 Spalten × 5 Reihen Matrix von Artikel-Buttons (konfiguriert via articles.yaml).
 * - Rechts (1/4 des Platzes): Übersichtspanel mit:
 *   - Gesamtpreis
 *   - Erhaltener Betrag (Tippen öffnet 789/456/123/0,CLR Dialog)
 *   - Rückgeld
 *   - Bezahlt (großer Aktionsknopf, wechselt dynamisch auf Grün sobald Betrag ausreicht)
 *   - Abbruch (immer roter Aktionsknopf, Zurücksetzen)
 */
class MainWindow : public Gtk::Window {
public:
    MainWindow();
    ~MainWindow() override = default;

private:
    void setup_ui();
    void load_articles();
    void update_overview();
    void reset_sale();
    void on_received_amount_clicked();
    void on_paid_clicked();
    void on_cancel_clicked();
    void update_paid_button_style(bool is_sufficient);

    // Zustand
    double m_total_price{0.0};
    int m_total_items{0};
    double m_received_amount{0.0};

    // Dialog-Referenz
    std::unique_ptr<CashInputDialog> m_cash_dialog;

    // Hauptcontainer (3/4 Links Matrix, 1/4 Rechts Übersicht)
    Gtk::Grid m_root_grid;

    // Linke Seite: 4x5 Artikel-Matrix
    Gtk::Box m_left_container{Gtk::Orientation::VERTICAL, 0};
    Gtk::Grid m_matrix_grid;
    std::vector<std::unique_ptr<ArticleButton>> m_article_buttons;

    // Rechte Seite: Abrechnung & Kassenübersicht
    Gtk::Box m_right_panel{Gtk::Orientation::VERTICAL, 14};
    Gtk::Label m_panel_title;

    // 1. Gesamtpreis-Karte
    Gtk::Box m_card_total{Gtk::Orientation::VERTICAL, 4};
    Gtk::Label m_lbl_total_title;
    Gtk::Label m_lbl_total_value;
    Gtk::Label m_lbl_total_count;

    // 2. Erhaltener Betrag (Klickbar -> Öffnet Eingabedialog)
    Gtk::Box m_card_received{Gtk::Orientation::VERTICAL, 4};
    Gtk::Label m_lbl_received_title;
    Gtk::Button m_btn_received_amount;
    Gtk::Label m_lbl_received_value;

    // 3. Rückgeld-Karte
    Gtk::Box m_card_change{Gtk::Orientation::VERTICAL, 4};
    Gtk::Label m_lbl_change_title;
    Gtk::Label m_lbl_change_value;

    // Spacer
    Gtk::Box m_bottom_spacer{Gtk::Orientation::VERTICAL};

    // Aktions-Knöpfe
    Gtk::Button m_btn_paid;   // Großer "Bezahlt"-Knopf (rot solange Betrag unzureichend, wechselt auf grün)
    Gtk::Label m_lbl_paid;
    Gtk::Button m_btn_cancel; // "Abbruch"-Knopf (immer rot)
    Gtk::Label m_lbl_cancel;

    // CSS Styling
    Glib::RefPtr<Gtk::CssProvider> m_css_provider;
    Glib::RefPtr<Gtk::CssProvider> m_paid_css_provider;
    Glib::RefPtr<Gtk::CssProvider> m_cancel_css_provider;
};

