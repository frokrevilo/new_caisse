#include "cash_input_dialog.hpp"
#include "article_button.hpp"
#include "scale_helper.hpp"

#include <gdk/gdkkeysyms.h>
#include <gtkmm/eventcontrollerkey.h>
#include <iomanip>
#include <sstream>
#include <algorithm>

CashInputDialog::CashInputDialog(Gtk::Window& parent, double total_due, double current_received)
    : m_total_due(total_due)
{
    auto& s = ScaleHelper::instance();
    set_transient_for(parent);
    set_modal(true);
    set_title("Erhaltener Betrag");
    set_default_size(s.px(600), s.px(900));
    set_resizable(false);

    if (current_received > 0.0) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << current_received;
        std::string s_val = oss.str();
        std::replace(s_val.begin(), s_val.end(), '.', ',');
        m_raw_input = s_val;
        m_replace_on_input = true; // Neue Zahl ersetzt den vorherigen Betrag
    } else {
        m_replace_on_input = false;
    }

    setup_ui();
    update_display();
}

CashInputDialog::~CashInputDialog()
{
    // Display-weiten Style-Provider sauber abmelden (direkt über den Standard-Display,
    // da das Window im Destruktor bereits entkoppelt/unrooted sein kann)
    auto display = Gdk::Display::get_default();
    if (display && m_css_provider) {
        Gtk::StyleProvider::remove_provider_for_display(display, m_css_provider);
    }
}

sigc::signal<void(double)>& CashInputDialog::signal_amount_confirmed()
{
    return m_signal_amount_confirmed;
}

void CashInputDialog::setup_ui()
{
    auto& s = ScaleHelper::instance();

    m_root_box.set_margin_start(s.px(28));
    m_root_box.set_margin_end(s.px(28));
    m_root_box.set_margin_top(s.px(24));
    m_root_box.set_margin_bottom(s.px(24));
    m_root_box.set_spacing(s.px(20));

    // 1. Kopfzeile: Titel und Zu zahlender Betrag
    m_header_label.set_markup("<span size='" + s.pt_str(26) + "' weight='bold' foreground='#ffffff'>Erhaltener Betrag</span>");
    m_root_box.append(m_header_label);

    const std::string due_str = ArticleButton::format_currency(m_total_due).raw();
    m_due_label.set_markup("<span size='" + s.pt_str(20) + "' foreground='#94a3b8'>Zu zahlen: <b>" + due_str + "</b></span>");
    m_root_box.append(m_due_label);

    // 2. Eingabe- & Rückgeld-Anzeigekarte
    m_display_card.add_css_class("cash-display-card");
    m_display_card.set_halign(Gtk::Align::FILL);

    m_display_label.set_halign(Gtk::Align::END);
    m_display_label.set_justify(Gtk::Justification::RIGHT);
    m_display_card.append(m_display_label);

    m_change_label.set_halign(Gtk::Align::END);
    m_change_label.set_justify(Gtk::Justification::RIGHT);
    m_display_card.append(m_change_label);

    m_root_box.append(m_display_card);

    // 4. Ziffernblock (4 Zeilen x 3 Spalten) - Quadratische Tasten
    // 7 8 9
    // 4 5 6
    // 1 2 3
    // 0 , CLR
    m_keypad_grid.set_row_spacing(s.px(16));
    m_keypad_grid.set_column_spacing(s.px(16));
    m_keypad_grid.set_row_homogeneous(true);
    m_keypad_grid.set_column_homogeneous(true);
    m_keypad_grid.set_halign(Gtk::Align::CENTER);
    m_keypad_grid.set_valign(Gtk::Align::CENTER);
    m_keypad_grid.set_hexpand(true);
    m_keypad_grid.set_vexpand(true);

    const struct KeyDef {
        const char* label;
        int col;
        int row;
        enum Type { DIGIT, COMMA, CLEAR } type;
        char digit;
    } keys[12] = {
        {"7",   0, 0, KeyDef::DIGIT, '7'},
        {"8",   1, 0, KeyDef::DIGIT, '8'},
        {"9",   2, 0, KeyDef::DIGIT, '9'},

        {"4",   0, 1, KeyDef::DIGIT, '4'},
        {"5",   1, 1, KeyDef::DIGIT, '5'},
        {"6",   2, 1, KeyDef::DIGIT, '6'},

        {"1",   0, 2, KeyDef::DIGIT, '1'},
        {"2",   1, 2, KeyDef::DIGIT, '2'},
        {"3",   2, 2, KeyDef::DIGIT, '3'},

        {"0",   0, 3, KeyDef::DIGIT, '0'},
        {",",   1, 3, KeyDef::COMMA, ','},
        {"CLR", 2, 3, KeyDef::CLEAR, ' '}
    };

    m_css_provider = Gtk::CssProvider::create();

    const int key_size = s.px(110);

    for (const auto& k : keys) {
        auto* btn = Gtk::make_managed<Gtk::Button>();
        auto* lbl = Gtk::make_managed<Gtk::Label>();

        if (k.type == KeyDef::CLEAR) {
            lbl->set_markup("<span size='" + s.pt_str(30) + "' weight='bold' foreground='#ffffff'>CLR</span>");
        } else if (k.type == KeyDef::COMMA) {
            lbl->set_markup("<span size='" + s.pt_str(44) + "' weight='bold' foreground='#0f172a'>,</span>");
        } else {
            lbl->set_markup("<span size='" + s.pt_str(38) + "' weight='bold' foreground='#0f172a'>" + std::string(k.label) + "</span>");
        }
        btn->set_child(*lbl);
        btn->set_size_request(key_size, key_size);
        btn->set_hexpand(false);
        btn->set_vexpand(false);
        btn->set_halign(Gtk::Align::CENTER);
        btn->set_valign(Gtk::Align::CENTER);
        btn->add_css_class("keypad-btn");

        if (k.type == KeyDef::CLEAR) {
            btn->set_name("btn-clr");
            btn->add_css_class("keypad-btn-clr");
            btn->signal_clicked().connect(sigc::mem_fun(*this, &CashInputDialog::on_clear_clicked));
        } else if (k.type == KeyDef::COMMA) {
            btn->signal_clicked().connect(sigc::mem_fun(*this, &CashInputDialog::on_comma_clicked));
        } else {
            const char dig = k.digit;
            btn->signal_clicked().connect([this, dig]() {
                on_digit_clicked(dig);
            });
        }

        m_keypad_grid.attach(*btn, k.col, k.row, 1, 1);
    }
    m_root_box.append(m_keypad_grid);

    // 5. Aktionen: Abbrechen / Übernehmen (Groß und gut lesbar)
    m_action_box.set_halign(Gtk::Align::FILL);
    m_action_box.set_spacing(s.px(18));

    m_btn_cancel.set_name("btn-dialog-cancel");
    m_btn_cancel.set_hexpand(true);
    m_btn_cancel.set_size_request(-1, s.px(76));
    m_btn_cancel.add_css_class("btn-dialog-cancel");
    m_lbl_cancel.set_markup("<span size='" + s.pt_str(22) + "' weight='bold' foreground='#ffffff'>✕  Abbrechen</span>");
    m_btn_cancel.set_child(m_lbl_cancel);
    m_btn_cancel.signal_clicked().connect(sigc::mem_fun(*this, &CashInputDialog::on_cancel_clicked));
    m_action_box.append(m_btn_cancel);

    m_btn_confirm.set_name("btn-dialog-confirm");
    m_btn_confirm.set_hexpand(true);
    m_btn_confirm.set_size_request(-1, s.px(76));
    m_btn_confirm.add_css_class("btn-dialog-confirm");
    m_lbl_confirm.set_markup("<span size='" + s.pt_str(22) + "' weight='bold' foreground='#ffffff'>✓  Übernehmen</span>");
    m_btn_confirm.set_child(m_lbl_confirm);
    m_btn_confirm.signal_clicked().connect(sigc::mem_fun(*this, &CashInputDialog::on_confirm_clicked));
    m_action_box.append(m_btn_confirm);

    m_root_box.append(m_action_box);

    set_child(m_root_box);

    // 6. CSS Styling
    // Über den Display-weiten Provider greifen Klassen- und ID-Selektoren für das gesamte Dialogfenster.
    const std::string css =
        "window, dialog {\n"
        "    background-color: #0f172a;\n"
        "    color: #f8fafc;\n"
        "}\n"
        ".cash-display-card {\n"
        "    background-color: #1e293b;\n"
        "    color: #38bdf8;\n"
        "    border-radius: " + std::to_string(s.px(12)) + "px;\n"
        "    padding: " + std::to_string(s.px(12)) + "px " + std::to_string(s.px(18)) + "px;\n"
        "    box-shadow: inset 0 " + std::to_string(s.px(2)) + "px " + std::to_string(s.px(5)) + "px rgba(0, 0, 0, 0.45);\n"
        "}\n"
        ".keypad-btn {\n"
        "    border-radius: " + std::to_string(s.px(12)) + "px;\n"
        "    background-color: #f1f5f9;\n"
        "    background-image: none;\n"
        "    color: #0f172a;\n"
        "    border: 1px solid rgba(0, 0, 0, 0.2);\n"
        "    box-shadow: 0 " + std::to_string(s.px(4)) + "px " + std::to_string(s.px(6)) + "px rgba(0, 0, 0, 0.25);\n"
        "    transition: all 100ms ease;\n"
        "}\n"
        ".keypad-btn:hover {\n"
        "    background-color: #e2e8f0;\n"
        "    background-image: none;\n"
        "    box-shadow: 0 " + std::to_string(s.px(5)) + "px " + std::to_string(s.px(8)) + "px rgba(0, 0, 0, 0.3);\n"
        "}\n"
        ".keypad-btn:active {\n"
        "    box-shadow: 0 " + std::to_string(s.px(1)) + "px " + std::to_string(s.px(3)) + "px rgba(0, 0, 0, 0.2);\n"
        "    transform: translateY(" + std::to_string(s.px(2)) + "px);\n"
        "    background-color: #cbd5e1;\n"
        "    background-image: none;\n"
        "}\n"
        ".keypad-btn-clr, #btn-clr {\n"
        "    background-color: #dc2626;\n"
        "    background-image: none;\n"
        "    color: #ffffff;\n"
        "    border-radius: " + std::to_string(s.px(12)) + "px;\n"
        "    border: 1px solid rgba(0, 0, 0, 0.2);\n"
        "    box-shadow: 0 " + std::to_string(s.px(4)) + "px " + std::to_string(s.px(6)) + "px rgba(0, 0, 0, 0.35);\n"
        "}\n"
        ".keypad-btn-clr:hover, #btn-clr:hover {\n"
        "    background-color: #ef4444;\n"
        "    background-image: none;\n"
        "    box-shadow: 0 " + std::to_string(s.px(5)) + "px " + std::to_string(s.px(8)) + "px rgba(0, 0, 0, 0.4);\n"
        "}\n"
        ".keypad-btn-clr:active, #btn-clr:active {\n"
        "    background-color: #b91c1c;\n"
        "    background-image: none;\n"
        "    box-shadow: 0 " + std::to_string(s.px(1)) + "px " + std::to_string(s.px(3)) + "px rgba(0, 0, 0, 0.25);\n"
        "    transform: translateY(" + std::to_string(s.px(2)) + "px);\n"
        "}\n"
        ".btn-dialog-cancel, #btn-dialog-cancel {\n"
        "    border-radius: " + std::to_string(s.px(12)) + "px;\n"
        "    background-color: #dc2626;\n"
        "    background-image: none;\n"
        "    color: #ffffff;\n"
        "    border: 1px solid rgba(0, 0, 0, 0.2);\n"
        "    box-shadow: 0 " + std::to_string(s.px(4)) + "px " + std::to_string(s.px(6)) + "px rgba(0, 0, 0, 0.35);\n"
        "    transition: all 120ms ease;\n"
        "}\n"
        ".btn-dialog-cancel:hover, #btn-dialog-cancel:hover {\n"
        "    background-color: #ef4444;\n"
        "    background-image: none;\n"
        "    box-shadow: 0 " + std::to_string(s.px(5)) + "px " + std::to_string(s.px(8)) + "px rgba(0, 0, 0, 0.4);\n"
        "}\n"
        ".btn-dialog-cancel:active, #btn-dialog-cancel:active {\n"
        "    transform: translateY(" + std::to_string(s.px(2)) + "px);\n"
        "    background-color: #b91c1c;\n"
        "    background-image: none;\n"
        "    box-shadow: 0 " + std::to_string(s.px(1)) + "px " + std::to_string(s.px(3)) + "px rgba(0, 0, 0, 0.25);\n"
        "}\n"
        ".btn-dialog-confirm, #btn-dialog-confirm {\n"
        "    border-radius: " + std::to_string(s.px(12)) + "px;\n"
        "    background-color: #16a34a;\n"
        "    background-image: none;\n"
        "    color: #ffffff;\n"
        "    border: 1px solid rgba(0, 0, 0, 0.2);\n"
        "    box-shadow: 0 " + std::to_string(s.px(4)) + "px " + std::to_string(s.px(6)) + "px rgba(0, 0, 0, 0.35);\n"
        "    transition: all 120ms ease;\n"
        "}\n"
        ".btn-dialog-confirm:hover, #btn-dialog-confirm:hover {\n"
        "    background-color: #22c55e;\n"
        "    background-image: none;\n"
        "    box-shadow: 0 " + std::to_string(s.px(5)) + "px " + std::to_string(s.px(8)) + "px rgba(0, 0, 0, 0.4);\n"
        "}\n"
        ".btn-dialog-confirm:active, #btn-dialog-confirm:active {\n"
        "    transform: translateY(" + std::to_string(s.px(2)) + "px);\n"
        "    background-color: #15803d;\n"
        "    background-image: none;\n"
        "    box-shadow: 0 " + std::to_string(s.px(1)) + "px " + std::to_string(s.px(3)) + "px rgba(0, 0, 0, 0.25);\n"
        "}\n";
    m_css_provider->load_from_data(css);

    auto display = Gdk::Display::get_default();
    if (display) {
        Gtk::StyleProvider::add_provider_for_display(display, m_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    }

    // 6. Tastatursteuerung
    auto key_ctrl = Gtk::EventControllerKey::create();
    key_ctrl->signal_key_pressed().connect(
        [this](guint keyval, guint /*keycode*/, Gdk::ModifierType /*state*/) {
            if (keyval >= GDK_KEY_0 && keyval <= GDK_KEY_9) {
                on_digit_clicked(static_cast<char>('0' + (keyval - GDK_KEY_0)));
                return true;
            }
            if (keyval >= GDK_KEY_KP_0 && keyval <= GDK_KEY_KP_9) {
                on_digit_clicked(static_cast<char>('0' + (keyval - GDK_KEY_KP_0)));
                return true;
            }
            if (keyval == GDK_KEY_comma || keyval == GDK_KEY_period || keyval == GDK_KEY_KP_Decimal) {
                on_comma_clicked();
                return true;
            }
            if (keyval == GDK_KEY_BackSpace) {
                if (m_replace_on_input) {
                    check_replace_on_input();
                    update_display();
                } else if (!m_raw_input.empty()) {
                    m_raw_input.pop_back();
                    update_display();
                }
                return true;
            }
            if (keyval == GDK_KEY_Delete) {
                on_clear_clicked();
                return true;
            }
            if (keyval == GDK_KEY_Return || keyval == GDK_KEY_KP_Enter) {
                on_confirm_clicked();
                return true;
            }
            if (keyval == GDK_KEY_Escape) {
                on_cancel_clicked();
                return true;
            }
            return false;
        },
        false
    );
    add_controller(key_ctrl);
}

void CashInputDialog::check_replace_on_input()
{
    if (m_replace_on_input) {
        m_raw_input.clear();
        m_replace_on_input = false;
    }
}

void CashInputDialog::update_display()
{
    auto& s = ScaleHelper::instance();
    std::string text;
    if (m_raw_input.empty()) {
        text = "0,00 €";
    } else {
        text = m_raw_input + " €";
    }
    m_display_label.set_markup("<span size='" + s.pt_str(38) + "' weight='bold' foreground='#38bdf8'>" + text + "</span>");

    const double received = parse_input_to_double();
    const long long received_cents = std::lround(received * 100.0);
    const long long due_cents = std::lround(m_total_due * 100.0);

    if (received_cents <= 0 && due_cents <= 0) {
        m_change_label.set_markup("<span size='" + s.pt_str(20) + "' weight='bold' foreground='#94a3b8'>Rückgeld: 0,00 €</span>");
    } else if (received_cents >= due_cents) {
        const double change = static_cast<double>(received_cents - due_cents) / 100.0;
        const std::string change_str = ArticleButton::format_currency(change).raw();
        m_change_label.set_markup("<span size='" + s.pt_str(22) + "' weight='bold' foreground='#22c55e'>Rückgeld: " + change_str + "</span>");
    } else {
        const double missing = static_cast<double>(due_cents - received_cents) / 100.0;
        const std::string missing_str = ArticleButton::format_currency(missing).raw();
        m_change_label.set_markup("<span size='" + s.pt_str(20) + "' weight='bold' foreground='#ef4444'>Noch offen: " + missing_str + "</span>");
    }
}

void CashInputDialog::on_digit_clicked(char digit)
{
    check_replace_on_input();

    if (m_raw_input == "0" && digit != '0') {
        m_raw_input.clear();
    }

    const auto comma_pos = m_raw_input.find(',');
    if (comma_pos != std::string::npos) {
        // Maximal 2 Nachkommastellen erlauben
        if (m_raw_input.length() - comma_pos > 2) {
            return;
        }
    } else {
        // Maximal 6 Ziffern vor dem Komma
        if (m_raw_input.length() >= 6) {
            return;
        }
    }

    m_raw_input += digit;
    update_display();
}

void CashInputDialog::on_comma_clicked()
{
    check_replace_on_input();

    if (m_raw_input.empty()) {
        m_raw_input = "0,";
    } else if (m_raw_input.find(',') == std::string::npos) {
        m_raw_input += ",";
    }
    update_display();
}

void CashInputDialog::on_clear_clicked()
{
    m_raw_input.clear();
    m_replace_on_input = false;
    update_display();
}

double CashInputDialog::parse_input_to_double() const
{
    if (m_raw_input.empty()) {
        return 0.0;
    }
    std::string str = m_raw_input;
    std::replace(str.begin(), str.end(), ',', '.');
    try {
        return std::stod(str);
    } catch (...) {
        return 0.0;
    }
}

void CashInputDialog::on_confirm_clicked()
{
    const double amount = parse_input_to_double();
    m_signal_amount_confirmed.emit(amount);
    close();
}

void CashInputDialog::on_cancel_clicked()
{
    close();
}
