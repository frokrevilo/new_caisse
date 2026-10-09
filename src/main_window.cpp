#include "main_window.hpp"
#include <gdk/gdkkeysyms.h>
#include <gtkmm/eventcontrollerkey.h>
#include <iostream>
#include <cmath>

MainWindow::MainWindow()
{
    // Fullscreen-Modus aktivieren
    fullscreen();

    setup_ui();
    load_articles();
    update_overview();
}

void MainWindow::setup_ui()
{
    // ============================================================
    // HAUPTLAYOUT: 4 Spalten homogenes Raster
    // Spalte 0..2 (Spannweite 3) = 3/4 für Artikel-Matrix (links)
    // Spalte 3 (Spannweite 1) = 1/4 für Übersicht (rechts)
    // ============================================================
    m_root_grid.set_margin_start(16);
    m_root_grid.set_margin_end(16);
    m_root_grid.set_margin_top(16);
    m_root_grid.set_margin_bottom(16);
    m_root_grid.set_column_spacing(18);
    m_root_grid.set_column_homogeneous(true);
    m_root_grid.set_row_homogeneous(false);
    m_root_grid.set_hexpand(true);
    m_root_grid.set_vexpand(true);

    // ============================================================
    // LINKE SEITE: 4x5 Matrix der Artikel-Buttons (3/4 des Platzes)
    // ============================================================
    m_left_container.set_hexpand(true);
    m_left_container.set_vexpand(true);

    m_matrix_title.set_markup("<span size='x-large' weight='bold'>Kassensystem — Artikel</span>");
    m_matrix_title.set_halign(Gtk::Align::START);
    m_left_header.append(m_matrix_title);
    m_left_container.append(m_left_header);

    // 4 Spalten × 5 Reihen Grid
    m_matrix_grid.set_row_spacing(10);
    m_matrix_grid.set_column_spacing(10);
    m_matrix_grid.set_row_homogeneous(true);
    m_matrix_grid.set_column_homogeneous(true);
    m_matrix_grid.set_hexpand(true);
    m_matrix_grid.set_vexpand(true);
    m_left_container.append(m_matrix_grid);

    m_root_grid.attach(m_left_container, 0, 0, 3, 1);

    // ============================================================
    // RECHTE SEITE: Kassenübersicht / Abrechnung (1/4 des Platzes)
    // ============================================================
    m_right_panel.set_hexpand(true);
    m_right_panel.set_vexpand(true);
    m_right_panel.add_css_class("checkout-panel");

    m_panel_title.set_markup("<span size='20pt' weight='bold' foreground='#ffffff'>Abrechnung</span>");
    m_panel_title.set_halign(Gtk::Align::START);
    m_right_panel.append(m_panel_title);

    // 1. Karte: Gesamtpreis
    m_card_total.add_css_class("overview-card");
    m_lbl_total_title.set_markup("<span size='12pt' weight='bold' foreground='#94a3b8'>GESAMTPREIS</span>");
    m_lbl_total_title.set_halign(Gtk::Align::START);
    m_card_total.append(m_lbl_total_title);

    m_lbl_total_value.set_halign(Gtk::Align::START);
    m_card_total.append(m_lbl_total_value);

    m_lbl_total_count.set_halign(Gtk::Align::START);
    m_card_total.append(m_lbl_total_count);
    m_right_panel.append(m_card_total);

    // 2. Karte: Erhaltener Betrag (Tippen öffnet Eingabedialog)
    m_card_received.add_css_class("overview-card");
    m_lbl_received_title.set_markup("<span size='12pt' weight='bold' foreground='#94a3b8'>ERHALTENER BETRAG (Tippen)</span>");
    m_lbl_received_title.set_halign(Gtk::Align::START);
    m_card_received.append(m_lbl_received_title);

    m_btn_received_amount.add_css_class("btn-received-amount");
    m_btn_received_amount.set_child(m_lbl_received_value);
    m_btn_received_amount.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_received_amount_clicked));
    m_card_received.append(m_btn_received_amount);
    m_right_panel.append(m_card_received);

    // 3. Karte: Rückgeld
    m_card_change.add_css_class("overview-card");
    m_lbl_change_title.set_markup("<span size='12pt' weight='bold' foreground='#94a3b8'>RÜCKGELD</span>");
    m_lbl_change_title.set_halign(Gtk::Align::START);
    m_card_change.append(m_lbl_change_title);

    m_lbl_change_value.set_halign(Gtk::Align::START);
    m_card_change.append(m_lbl_change_value);
    m_right_panel.append(m_card_change);

    // Dehnbarer Leerraum schiebt Buttons nach unten
    m_bottom_spacer.set_vexpand(true);
    m_right_panel.append(m_bottom_spacer);

    // 4. Aktions-Knopf: BEZAHLT (rot solange Betrag unzureichend, grün sobald Betrag ausreicht)
    m_btn_paid.set_name("main-btn-paid");           // Eindeutige Widget-ID für CSS #main-btn-paid
    m_btn_paid.set_size_request(-1, 80);
    m_btn_paid.set_margin_bottom(28);
    m_lbl_paid.set_markup("<span size='22pt' weight='bold' foreground='#ffffff'>✓  BEZAHLT</span>");
    m_btn_paid.set_child(m_lbl_paid);
    m_btn_paid.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_paid_clicked));
    m_paid_css_provider = Gtk::CssProvider::create();
    // Provider wird unten global an den Display gehängt; erst danach update_paid_button_style aufrufen
    m_right_panel.append(m_btn_paid);

    // 5. Aktions-Knopf: Abbruch (immer rot, zentriert und schmaler als Bezahlt-Knopf)
    m_btn_cancel.set_name("main-btn-cancel");       // Eindeutige Widget-ID für CSS #main-btn-cancel
    m_btn_cancel.set_size_request(180, 46);
    m_btn_cancel.set_halign(Gtk::Align::CENTER);
    m_btn_cancel.set_margin_bottom(6);
    m_lbl_cancel.set_markup("<span size='15pt' weight='bold' foreground='#ffffff'>✕  Abbruch</span>");
    m_btn_cancel.set_child(m_lbl_cancel);
    m_btn_cancel.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_cancel_clicked));
    m_cancel_css_provider = Gtk::CssProvider::create();
    m_cancel_css_provider->load_from_data(R"(
        #main-btn-cancel {
            background-color: #dc2626;
            background-image: none;
            background: #dc2626;
            color: #ffffff;
            border-radius: 10px;
            border: 1px solid rgba(0, 0, 0, 0.2);
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.35);
            transition: all 120ms ease;
        }
        #main-btn-cancel:hover {
            background-color: #ef4444;
            background-image: none;
            background: #ef4444;
            box-shadow: 0 5px 8px rgba(0, 0, 0, 0.4);
        }
        #main-btn-cancel:active {
            transform: translateY(2px);
            background-color: #b91c1c;
            background-image: none;
            background: #b91c1c;
            box-shadow: 0 1px 3px rgba(0, 0, 0, 0.25);
        }
    )");
    m_right_panel.append(m_btn_cancel);

    m_root_grid.attach(m_right_panel, 3, 0, 1, 1);

    set_child(m_root_grid);

    // ============================================================
    // Globales CSS Styling – alle Provider an den Display hängen
    // (ID-Selektoren wie #main-btn-paid matchen nur global im Display-CSS-Baum)
    // ============================================================
    m_css_provider = Gtk::CssProvider::create();
    const std::string css = R"(
        .checkout-panel {
            background-color: #1e293b;
            border-radius: 14px;
            padding: 16px;
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.35);
        }
        .overview-card {
            background-color: #0f172a;
            border-radius: 10px;
            padding: 12px 16px;
            border: 1px solid rgba(255, 255, 255, 0.08);
        }
        .btn-received-amount {
            background-color: #334155;
            border: 1px solid #475569;
            border-radius: 8px;
            padding: 10px 14px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
            transition: all 120ms ease;
        }
        .btn-received-amount:hover {
            background-color: #3b4e68;
            border-color: #64748b;
        }
        .btn-received-amount:active {
            transform: translateY(2px);
            box-shadow: 0 1px 2px rgba(0, 0, 0, 0.2);
        }
    )";
    m_css_provider->load_from_data(css);
    get_style_context()->add_provider(m_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    auto display = Gdk::Display::get_default();
    if (display) {
        // Globale Panel-Styles
        Gtk::StyleProvider::add_provider_for_display(display, m_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
        // Abbruch-Knopf: Immer rot, via #main-btn-cancel (ID-Selektor braucht Display-Scope)
        Gtk::StyleProvider::add_provider_for_display(display, m_cancel_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
        // Bezahlt-Knopf: Dynamisch rot/grün (Provider bleibt am Display, CSS wird per load_from_data aktualisiert)
        Gtk::StyleProvider::add_provider_for_display(display, m_paid_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    }

    // Erst NACH dem Display-Register den initialen Stil setzen
    update_paid_button_style(false);

    // ============================================================
    // Tastatur-Controller (Escape = Beenden, F11 = Vollbild umschalten)
    // ============================================================
    auto key_ctrl = Gtk::EventControllerKey::create();
    key_ctrl->signal_key_pressed().connect(
        [this](guint keyval, guint /*keycode*/, Gdk::ModifierType /*state*/) {
            if (keyval == GDK_KEY_Escape) {
                close();
                return true;
            }
            if (keyval == GDK_KEY_F11) {
                if (is_fullscreen()) {
                    unfullscreen();
                } else {
                    fullscreen();
                }
                return true;
            }
            return false;
        },
        false
    );
    add_controller(key_ctrl);
}

void MainWindow::load_articles()
{
    // Konfiguration laden (erstellt Datei articles.yaml automatisch, falls nicht vorhanden)
    const std::string yaml_file = "articles.yaml";
    const auto configs = load_articles_from_yaml(yaml_file);

    // Grid leeren (falls zuvor befüllt)
    for (auto& btn : m_article_buttons) {
        m_matrix_grid.remove(*btn);
    }
    m_article_buttons.clear();

    // Matrix (4 Spalten x 5 Reihen) befüllen
    for (const auto& cfg : configs) {
        auto btn = std::make_unique<ArticleButton>(cfg.name, cfg.price, cfg.color);
        btn->set_visible(cfg.visible);

        // Klick auf Artikel aktualisiert die Gesamtübersicht
        btn->signal_clicked().connect([this]() {
            update_overview();
        });

        // Langer Druck (>3s) setzt Artikel auf 0 zurück und aktualisiert die Gesamtübersicht
        btn->signal_long_pressed().connect([this]() {
            update_overview();
        });

        m_matrix_grid.attach(*btn, cfg.x, cfg.y, 1, 1);
        m_article_buttons.push_back(std::move(btn));
    }
}

void MainWindow::update_overview()
{
    m_total_price = 0.0;
    m_total_items = 0;

    for (const auto& btn : m_article_buttons) {
        m_total_price += btn->get_totalprice();
        m_total_items += btn->get_amount();
    }

    // 1. Gesamtpreis-Anzeige (deutlich größere Schrift)
    const std::string total_str = ArticleButton::format_currency(m_total_price).raw();
    m_lbl_total_value.set_markup(
        "<span size='32pt' weight='bold' foreground='#ffffff'>" + total_str + "</span>"
    );

    const std::string count_str = "(" + std::to_string(m_total_items) +
                                  (m_total_items == 1 ? " Artikel ausgewählt)" : " Artikel ausgewählt)");
    m_lbl_total_count.set_markup(
        "<span size='13pt' foreground='#94a3b8'>" + count_str + "</span>"
    );

    // 2. Erhaltener Betrag Anzeige (deutlich größere Schrift)
    const std::string received_str = ArticleButton::format_currency(m_received_amount).raw();
    m_lbl_received_value.set_markup(
        "<span size='28pt' weight='bold' foreground='#38bdf8'>" + received_str + "</span>"
    );

    const long total_cents = std::lround(m_total_price * 100.0);
    const long received_cents = std::lround(m_received_amount * 100.0);

    // 3. Rückgeld-Anzeige (deutlich größere Schrift)
    if (received_cents <= 0 && total_cents <= 0) {
        m_lbl_change_value.set_markup(
            "<span size='32pt' weight='bold' foreground='#94a3b8'>0,00 €</span>"
        );
    } else if (received_cents >= total_cents) {
        const double change = (received_cents - total_cents) / 100.0;
        const std::string change_str = ArticleButton::format_currency(change).raw();
        m_lbl_change_value.set_markup(
            "<span size='32pt' weight='bold' foreground='#22c55e'>+ " + change_str + "</span>"
        );
    } else {
        const double missing = (total_cents - received_cents) / 100.0;
        const std::string missing_str = ArticleButton::format_currency(missing).raw();
        m_lbl_change_value.set_markup(
            "<span size='26pt' weight='bold' foreground='#ef4444'>Noch: " + missing_str + "</span>"
        );
    }

    // 4. Farbe des Bezahlt-Knopfes: Rot solange unzureichend, wechselt auf Grün sobald erhaltener Betrag ausreicht
    const bool is_sufficient = (total_cents > 0 && received_cents >= total_cents);
    update_paid_button_style(is_sufficient);
}

void MainWindow::update_paid_button_style(bool is_sufficient)
{
    const std::string bg = is_sufficient ? "#16a34a" : "#dc2626";
    const std::string hover = is_sufficient ? "#22c55e" : "#ef4444";
    const std::string active = is_sufficient ? "#15803d" : "#b91c1c";

    const std::string css = 
        "#main-btn-paid {\n"
        "    background-color: " + bg + ";\n"
        "    background-image: none;\n"
        "    background: " + bg + ";\n"
        "    color: #ffffff;\n"
        "    border-radius: 12px;\n"
        "    border: 1px solid rgba(0, 0, 0, 0.2);\n"
        "    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.35);\n"
        "    transition: all 120ms ease;\n"
        "}\n"
        "#main-btn-paid:hover {\n"
        "    background-color: " + hover + ";\n"
        "    background-image: none;\n"
        "    background: " + hover + ";\n"
        "    box-shadow: 0 6px 10px rgba(0, 0, 0, 0.4);\n"
        "}\n"
        "#main-btn-paid:active {\n"
        "    background-color: " + active + ";\n"
        "    background-image: none;\n"
        "    background: " + active + ";\n"
        "    box-shadow: 0 1px 3px rgba(0, 0, 0, 0.25);\n"
        "    transform: translateY(2px);\n"
        "}\n";
    m_paid_css_provider->load_from_data(css);
}

void MainWindow::on_received_amount_clicked()
{
    // Öffnet den Ziffernblock-Dialog (789 / 456 / 123 / 0,CLR)
    m_cash_dialog = std::make_unique<CashInputDialog>(*this, m_total_price, m_received_amount);

    m_cash_dialog->signal_amount_confirmed().connect([this](double amount) {
        m_received_amount = amount;
        update_overview();
    });

    m_cash_dialog->present();
}

void MainWindow::on_paid_clicked()
{
    const long total_cents = std::lround(m_total_price * 100.0);
    const long received_cents = std::lround(m_received_amount * 100.0);

    if (total_cents <= 0) {
        return;
    }
    // Falls noch nicht genug bezahlt wurde, direkt Eingabedialog öffnen
    if (received_cents < total_cents) {
        on_received_amount_clicked();
        return;
    }

    // Buchung abschließen: alle Artikelzähler und erhaltenen Betrag zurücksetzen
    for (auto& btn : m_article_buttons) {
        btn->reset_amount();
    }
    m_received_amount = 0.0;
    update_overview();
}

void MainWindow::on_cancel_clicked()
{
    // Buchung abbrechen: alle Artikelzähler und erhaltenen Betrag zurücksetzen
    for (auto& btn : m_article_buttons) {
        btn->reset_amount();
    }
    m_received_amount = 0.0;
    update_overview();
}

