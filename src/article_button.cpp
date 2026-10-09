#include "article_button.hpp"

#include <gdkmm/rgba.h>
#include <glibmm/main.h>
#include <glibmm/markup.h>
#include <gtkmm/gestureclick.h>
#include <gtkmm/eventcontrollermotion.h>
#include <algorithm>
#include <iomanip>
#include <sstream>

ArticleButton::ArticleButton()
{
    setup_ui();
}

ArticleButton::ArticleButton(const Glib::ustring& article, double price, const Glib::ustring& color)
    : m_article(article)
    , m_color(color)
    , m_price(price)
{
    setup_ui();
}

ArticleButton::~ArticleButton()
{
    if (m_long_press_connection.connected()) {
        m_long_press_connection.disconnect();
    }
}

void ArticleButton::setup_ui()
{
    // Eindeutige CSS-ID für jeden Button generieren
    static unsigned int s_counter = 0;
    m_css_id = "article-btn-" + std::to_string(++s_counter);
    set_name(m_css_id);
    add_css_class("article-button");

    // Standardgröße für Touch-/POS-Bedienung in der 4x5 Matrix
    set_size_request(130, 70);
    set_hexpand(true);
    set_vexpand(true);

    // Eigener CSS-Provider für instanzspezifische Farb- und Stilregeln
    m_css_provider = Gtk::CssProvider::create();
    get_style_context()->add_provider(m_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    m_amount_label.get_style_context()->add_provider(m_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Vertikaler Hauptcontainer
    m_root_box.set_spacing(4);
    m_root_box.set_can_target(false);
    m_root_box.set_margin_start(4);
    m_root_box.set_margin_end(4);
    m_root_box.set_margin_top(2);
    m_root_box.set_margin_bottom(2);

    // Obere Leiste: Links Klickzähler, Rechts Preis
    m_top_box.set_spacing(4);
    m_top_box.set_can_target(false);

    // Klickzähler-Badge (oben links)
    m_amount_label.set_halign(Gtk::Align::START);
    m_amount_label.set_valign(Gtk::Align::CENTER);
    m_amount_label.add_css_class("article-btn-amount");
    m_amount_label.set_can_target(false);
    m_amount_label.set_visible(true);
    m_top_box.append(m_amount_label);

    // Dehnbarer Zwischenraum drückt den Preis nach rechts
    auto top_spacer = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    top_spacer->set_hexpand(true);
    top_spacer->set_can_target(false);
    m_top_box.append(*top_spacer);

    // Preis-Label (oben rechts)
    m_price_label.set_halign(Gtk::Align::END);
    m_price_label.set_valign(Gtk::Align::CENTER);
    m_price_label.add_css_class("article-btn-price");
    m_price_label.set_can_target(false);
    m_top_box.append(m_price_label);

    m_root_box.append(m_top_box);

    // Zentrale Artikelbeschriftung
    m_article_label.set_halign(Gtk::Align::CENTER);
    m_article_label.set_valign(Gtk::Align::CENTER);
    m_article_label.set_hexpand(true);
    m_article_label.set_vexpand(true);
    m_article_label.set_wrap(true);
    m_article_label.set_justify(Gtk::Justification::CENTER);
    m_article_label.add_css_class("article-btn-article");
    m_article_label.set_can_target(false);
    m_root_box.append(m_article_label);

    // Unterer Abstandshalter zur optischen Zentrierung der Artikelbeschriftung
    m_bottom_spacer.set_can_target(false);
    m_bottom_spacer.set_size_request(-1, 14);
    m_root_box.append(m_bottom_spacer);

    set_child(m_root_box);

    // Controller für langen Tastendruck (>3s)
    auto gesture = Gtk::GestureClick::create();
    gesture->set_propagation_phase(Gtk::PropagationPhase::CAPTURE);

    gesture->signal_pressed().connect([this](int /*n_press*/, double /*x*/, double /*y*/) {
        if (m_long_press_connection.connected()) {
            m_long_press_connection.disconnect();
        }
        m_long_press_triggered = false;

        // 3-Sekunden-Timer starten
        m_long_press_connection = Glib::signal_timeout().connect(
            [this]() {
                m_long_press_triggered = true;
                reset_amount();
                m_signal_long_pressed.emit();
                g_print("ArticleButton [%s]: Langer Druck erkannt (>3s) -> Zähler auf 0 zurückgesetzt.\n", m_article.c_str());
                return false; // Einmalig ausführen
            },
            m_long_press_duration_ms
        );
    });

    // Erst beim Loslassen der Maustaste/Touch abbrechen (sofern noch nicht abgelaufen)
    gesture->signal_released().connect([this](int /*n_press*/, double /*x*/, double /*y*/) {
        if (m_long_press_connection.connected()) {
            m_long_press_connection.disconnect();
        }
    });
    add_controller(gesture);

    // Bricht den Timer ab, falls der Cursor den Button während des Haltens verlässt
    auto motion = Gtk::EventControllerMotion::create();
    motion->signal_leave().connect([this]() {
        if (m_long_press_connection.connected()) {
            m_long_press_connection.disconnect();
        }
    });
    add_controller(motion);

    update_style();
    update_display();
}

void ArticleButton::set_article(const Glib::ustring& article)
{
    m_article = article;
    update_display();
}

Glib::ustring ArticleButton::get_article() const
{
    return m_article;
}

void ArticleButton::set_color(const Glib::ustring& color)
{
    m_color = color;
    update_style();
    update_display();
}

Glib::ustring ArticleButton::get_color() const
{
    return m_color;
}

void ArticleButton::set_price(double price)
{
    m_price = price;
    update_display();
}

double ArticleButton::get_price() const
{
    return m_price;
}

int ArticleButton::get_amount() const
{
    return m_amount;
}

void ArticleButton::reset_amount()
{
    m_amount = 0;
    update_display();
}

void ArticleButton::reset()
{
    reset_amount();
}

void ArticleButton::set_amount(int amount)
{
    m_amount = std::max(0, amount);
    update_display();
}

double ArticleButton::get_totalprice() const
{
    return static_cast<double>(m_amount) * m_price;
}

double ArticleButton::get_total_price() const
{
    return get_totalprice();
}

void ArticleButton::set_currency_symbol(const Glib::ustring& symbol)
{
    m_currency_symbol = symbol;
    update_display();
}

Glib::ustring ArticleButton::get_currency_symbol() const
{
    return m_currency_symbol;
}

void ArticleButton::set_article_font_size(const Glib::ustring& size)
{
    m_article_font_size = size;
    update_display();
}

Glib::ustring ArticleButton::get_article_font_size() const
{
    return m_article_font_size;
}

void ArticleButton::set_price_font_size(const Glib::ustring& size)
{
    m_price_font_size = size;
    update_display();
}

Glib::ustring ArticleButton::get_price_font_size() const
{
    return m_price_font_size;
}

void ArticleButton::set_amount_font_size(const Glib::ustring& size)
{
    m_amount_font_size = size;
    update_style();
    update_display();
}

Glib::ustring ArticleButton::get_amount_font_size() const
{
    return m_amount_font_size;
}

void ArticleButton::set_long_press_duration_ms(unsigned int ms)
{
    m_long_press_duration_ms = ms;
}

unsigned int ArticleButton::get_long_press_duration_ms() const
{
    return m_long_press_duration_ms;
}

sigc::signal<void()>& ArticleButton::signal_long_pressed()
{
    return m_signal_long_pressed;
}

void ArticleButton::on_clicked()
{
    // Falls ein langer Tastendruck (>3s) ausgelöst wurde, normalen Klick unterdrücken
    if (m_long_press_triggered) {
        m_long_press_triggered = false;
        return;
    }

    m_amount++;
    update_display();

    // Basisklassen-Methode aufrufen, damit externe signal_clicked()-Slots aufgerufen werden
    Gtk::Button::on_clicked();
}

bool ArticleButton::is_light_background() const
{
    if (m_color.empty()) {
        return false;
    }
    Gdk::RGBA rgba;
    if (rgba.set(m_color)) {
        double luminance = 0.299 * rgba.get_red() + 0.587 * rgba.get_green() + 0.114 * rgba.get_blue();
        return luminance > 0.55;
    }
    return false;
}

void ArticleButton::update_display()
{
    // Textfarbe für Pango-Markup bestimmen
    const bool light = is_light_background();
    const std::string fg_attr = !m_color.empty()
        ? (light ? " foreground='#1c1c1e'" : " foreground='#ffffff'")
        : "";

    // 1. Artikelbeschriftung
    const std::string escaped_article = Glib::Markup::escape_text(m_article);
    const std::string article_markup = "<span weight='bold' size='" + m_article_font_size + "'" + fg_attr + ">" +
                                       escaped_article + "</span>";
    m_article_label.set_markup(article_markup);

    // 2. Preis-Label
    const std::string escaped_price = Glib::Markup::escape_text(format_currency(m_price, m_currency_symbol));
    const std::string price_markup = "<span weight='bold' size='" + m_price_font_size + "'" + fg_attr + ">" +
                                     escaped_price + "</span>";
    m_price_label.set_markup(price_markup);

    // 3. Klickzähler-Badge: Dauerhaft sichtbar als reine Zahl "0", "1", "2" etc.
    const std::string amount_text = std::to_string(m_amount);
    const std::string amount_markup = "<span weight='bold' size='" + m_amount_font_size + "'>" + amount_text + "</span>";
    m_amount_label.set_markup(amount_markup);

    // 4. Informativer Tooltip
    if (!m_article.empty()) {
        if (m_amount > 0) {
            std::string tip = m_article + ": " + std::to_string(m_amount) + " × " +
                              format_currency(m_price, m_currency_symbol).raw() + " = " +
                              format_currency(get_totalprice(), m_currency_symbol).raw();
            set_tooltip_text(tip);
        } else {
            set_tooltip_text(m_article + " (" + format_currency(m_price, m_currency_symbol).raw() + ")");
        }
    } else {
        set_tooltip_text("");
    }
}

void ArticleButton::update_style()
{
    std::ostringstream css;

    // Basis-Button-Styling mit 3D-Schlagschatten und Übergangsanimation
    css << "#" << m_css_id << " {\n"
        << "    border-radius: 10px;\n"
        << "    padding: 6px 10px;\n"
        << "    margin: 3px;\n"
        << "    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.35), 0 1px 3px rgba(0, 0, 0, 0.2);\n"
        << "    transition: all 120ms ease;\n";

    if (!m_color.empty()) {
        css << "    background-color: " << m_color << ";\n"
            << "    background-image: none;\n"
            << "    border: 1px solid rgba(0, 0, 0, 0.22);\n";
    }
    css << "}\n"
        << "#" << m_css_id << ":hover {\n"
        << "    box-shadow: 0 6px 12px rgba(0, 0, 0, 0.42), 0 2px 4px rgba(0, 0, 0, 0.25);\n"
        << "    filter: brightness(1.08);\n"
        << "}\n"
        << "#" << m_css_id << ":active {\n"
        << "    box-shadow: 0 1px 2px rgba(0, 0, 0, 0.25);\n"
        << "    transform: translateY(3px);\n"
        << "    filter: brightness(0.93);\n"
        << "}\n";

    if (!m_color.empty()) {
        const bool dark_text = is_light_background();
        // Badge für Klickzähler passend zum Kontrast einfärben
        css << "label.article-btn-amount, #" << m_css_id << " .article-btn-amount {\n"
            << "    border-radius: 10px;\n"
            << "    padding: 1px 7px;\n"
            << "    background-color: " << (dark_text ? "rgba(0, 0, 0, 0.16)" : "rgba(255, 255, 255, 0.28)") << ";\n"
            << "    color: " << (dark_text ? "#1c1c1e" : "#ffffff") << ";\n"
            << "}\n";
    } else {
        css << "label.article-btn-amount, #" << m_css_id << " .article-btn-amount {\n"
            << "    border-radius: 10px;\n"
            << "    padding: 1px 7px;\n"
            << "    background-color: rgba(127, 127, 127, 0.25);\n"
            << "}\n";
    }

    m_css_provider->load_from_data(css.str());
}

Glib::ustring ArticleButton::format_currency(double value, const Glib::ustring& symbol)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    std::string str = oss.str();
    std::replace(str.begin(), str.end(), '.', ',');
    return Glib::ustring(str) + " " + symbol;
}
