#pragma once

#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/cssprovider.h>
#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>
#include <memory>

/**
 * @brief Eine anpassbare Button-Klasse für POS-/Kassensysteme abgeleitet von Gtk::Button.
 *
 * Eigenschaften:
 * - article: Zentrale Artikelbeschriftung (z.B. "Kaffee", "Tee").
 * - color: Farbgebung des Buttons mit automatischer Kontrastanpassung.
 * - price: Einzelpreis, rechtsbündig im oberen Bereich des Buttons.
 * - amount: Klickzähler mit dauerhafter Badge-Anzeige oben links (z. B. "0", "1").
 * - totalprice: Gesamtpreis (amount * price).
 * - long press (>3s): Setzt die Anzahl für diesen Button auf 0 zurück.
 */
class ArticleButton : public Gtk::Button {
public:
    ArticleButton();
    ArticleButton(const Glib::ustring& article, double price, const Glib::ustring& color = "");
    ~ArticleButton() override;

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
    void reset(); // Alias für reset_amount()
    void set_amount(int amount);

    // --- Total price ---
    [[nodiscard]] double get_totalprice() const;
    [[nodiscard]] double get_total_price() const; // Alias mit Unterstrich

    // --- Währungssymbol & Währungsformatierung ---
    void set_currency_symbol(const Glib::ustring& symbol);
    [[nodiscard]] Glib::ustring get_currency_symbol() const;
    static Glib::ustring format_currency(double value, const Glib::ustring& symbol = "€");

    // --- Schriftgrößen (z. B. "200%", "24pt", "14pt", "12pt") ---
    void set_article_font_size(const Glib::ustring& size);
    [[nodiscard]] Glib::ustring get_article_font_size() const;

    void set_price_font_size(const Glib::ustring& size);
    [[nodiscard]] Glib::ustring get_price_font_size() const;

    void set_amount_font_size(const Glib::ustring& size);
    [[nodiscard]] Glib::ustring get_amount_font_size() const;

    // --- Langer Tastendruck (>3s) ---
    void set_long_press_duration_ms(unsigned int ms);
    [[nodiscard]] unsigned int get_long_press_duration_ms() const;
    sigc::signal<void()>& signal_long_pressed();

protected:
    void on_clicked() override;

private:
    void setup_ui();
    void update_display();
    void update_style();
    [[nodiscard]] bool is_light_background() const;

    // Datenfelder
    Glib::ustring m_article;
    Glib::ustring m_color;
    double m_price{0.0};
    int m_amount{0};
    Glib::ustring m_currency_symbol{"€"};
    Glib::ustring m_css_id;

    // Standard-Schriftgrößen (hier anpassbar)
    Glib::ustring m_article_font_size{"16pt"}; // Artikelbeschriftung
    Glib::ustring m_price_font_size{"11pt"};    // Preisbeschriftung
    Glib::ustring m_amount_font_size{"11pt"};   // Klickzähler-Badge

    // Langer Druck
    unsigned int m_long_press_duration_ms{2000};
    bool m_long_press_triggered{false};
    sigc::connection m_long_press_connection;
    sigc::signal<void()> m_signal_long_pressed;

    // UI-Elemente
    Gtk::Box m_root_box{Gtk::Orientation::VERTICAL};
    Gtk::Box m_top_box{Gtk::Orientation::HORIZONTAL};
    Gtk::Label m_amount_label;
    Gtk::Label m_price_label;
    Gtk::Label m_article_label;
    Gtk::Box m_bottom_spacer{Gtk::Orientation::HORIZONTAL};

    // Style-Provider für individuelles Button-Styling
    Glib::RefPtr<Gtk::CssProvider> m_css_provider;
};
