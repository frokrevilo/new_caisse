#pragma once

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/cssprovider.h>
#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>
#include <string>

/**
 * @brief Artikel-Kachel für das Kassensystem abgeleitet von Gtk::Box.
 *
 * Aufbau der Kachel:
 * - Oben:
 *   - Artikelname (zentriert, umbrochen, steht für sich alleine)
 * - Mitte:
 *   - Haupt-Inkrement-Button ("+"): Verwendet die konfigurierte Artikelfarbe
 *   - Rechts daneben verkleinerter Dekrement-Button ("-"): Verwendet dieselbe Farbe, aber aufgehellt/dezent
 * - Unten:
 *   - Klickzähler / Badge (links unten)
 *   - Einzelpreis (rechts unten)
 */
class ArticleButton : public Gtk::Box {
public:
    ArticleButton();
    ArticleButton(const Glib::ustring& article, double price, const Glib::ustring& color = "");
    ~ArticleButton() override = default;

    // --- Article ---
    void set_article(const Glib::ustring& article);
    [[nodiscard]] Glib::ustring get_article() const;

    // --- Color ---
    void set_color(const Glib::ustring& color);
    [[nodiscard]] Glib::ustring get_color() const;

    // --- Price ---
    void set_price(double price);
    [[nodiscard]] double get_price() const;

    // --- Amount (Klickzähler) ---
    [[nodiscard]] int get_amount() const;
    void reset_amount();
    void set_amount(int amount);

    // --- Total price ---
    [[nodiscard]] double get_total_price() const;

    // --- Währungssymbol & Währungsformatierung ---
    void set_currency_symbol(const Glib::ustring& symbol);
    [[nodiscard]] Glib::ustring get_currency_symbol() const;
    static Glib::ustring format_currency(double value, const Glib::ustring& symbol = "€");

    // --- Schriftgrößen ---
    void set_article_font_size(const Glib::ustring& size);
    [[nodiscard]] Glib::ustring get_article_font_size() const;

    void set_price_font_size(const Glib::ustring& size);
    [[nodiscard]] Glib::ustring get_price_font_size() const;

    void set_amount_font_size(const Glib::ustring& size);
    [[nodiscard]] Glib::ustring get_amount_font_size() const;

    /// Signal wird ausgelöst, wenn "+" oder "-" betätigt wird (Mengenänderung).
    sigc::signal<void()>& signal_clicked();

private:
    void setup_ui();
    void update_display();
    void update_style();
    void on_plus_clicked();
    void on_minus_clicked();

    [[nodiscard]] static bool is_light_color(const std::string& hex);
    [[nodiscard]] static std::string lighten_color(const std::string& hex, double factor = 0.40);

    // Datenfelder
    Glib::ustring m_article;
    Glib::ustring m_color;
    double m_price{0.0};
    int m_amount{0};
    Glib::ustring m_currency_symbol{"€"};
    Glib::ustring m_css_id;

    // Standard-Schriftgrößen (proportional durch ScaleHelper initialisiert)
    Glib::ustring m_article_font_size{"16pt"};
    Glib::ustring m_price_font_size{"16pt"};
    Glib::ustring m_amount_font_size{"16pt"};

    // Signale
    sigc::signal<void()> m_signal_clicked;

    // UI-Elemente
    Gtk::Box m_header_box{Gtk::Orientation::HORIZONTAL};
    Gtk::Label m_article_label;

    Gtk::Box m_button_box{Gtk::Orientation::HORIZONTAL};
    Gtk::Button m_btn_plus;
    Gtk::Label m_lbl_plus;
    Gtk::Button m_btn_minus;
    Gtk::Label m_lbl_minus;

    Gtk::Box m_footer_box{Gtk::Orientation::HORIZONTAL};
    Gtk::Label m_amount_label;
    Gtk::Label m_price_label;

    // Style-Provider für individuelles Button-Styling
    Glib::RefPtr<Gtk::CssProvider> m_css_provider;
};
