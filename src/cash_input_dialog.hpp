#pragma once

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/grid.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/cssprovider.h>
#include <sigc++/sigc++.h>
#include <string>

/**
 * @brief Touch-fähiger Dialog zur Eingabe des erhaltenen Bargeldbetrags.
 *
 * Tastaturlayout:
 * 7  8  9
 * 4  5  6
 * 1  2  3
 * 0  ,  CLR
 *
 * Aktionen: Abbrechen, Übernehmen
 */
class CashInputDialog : public Gtk::Window {
public:
    CashInputDialog(Gtk::Window& parent, double total_due, double current_received = 0.0);
    ~CashInputDialog() override;

    /// Signal wird ausgelöst, wenn der Betrag bestätigt ("Übernehmen") wird.
    sigc::signal<void(double)>& signal_amount_confirmed();

private:
    void setup_ui();
    void update_display();
    void check_replace_on_input();
    void on_digit_clicked(char digit);
    void on_comma_clicked();
    void on_clear_clicked();
    void on_confirm_clicked();
    void on_cancel_clicked();
    [[nodiscard]] double parse_input_to_double() const;

    double m_total_due{0.0};
    std::string m_raw_input;
    bool m_replace_on_input{false};
    sigc::signal<void(double)> m_signal_amount_confirmed;

    // UI-Elemente
    Gtk::Box m_root_box{Gtk::Orientation::VERTICAL, 14};
    Gtk::Label m_header_label;
    Gtk::Label m_due_label;
    Gtk::Box m_display_card{Gtk::Orientation::VERTICAL, 4};
    Gtk::Label m_display_label;
    Gtk::Label m_change_label; // Live Rückgeld-Anzeige im Dialog
    Gtk::Grid m_keypad_grid;
    Gtk::Box m_action_box{Gtk::Orientation::HORIZONTAL, 14};

    Gtk::Button m_btn_cancel;
    Gtk::Label m_lbl_cancel;
    Gtk::Button m_btn_confirm;
    Gtk::Label m_lbl_confirm;

    Glib::RefPtr<Gtk::CssProvider> m_css_provider;
};

