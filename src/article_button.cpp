#include "article_button.hpp"
#include "scale_helper.hpp"

#include <gdkmm/rgba.h>
#include <glibmm/markup.h>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <cstdio>

ArticleButton::ArticleButton()
    : Gtk::Box(Gtk::Orientation::VERTICAL)
{
    setup_ui();
}

ArticleButton::ArticleButton(const Glib::ustring& article, double price, const Glib::ustring& color)
    : Gtk::Box(Gtk::Orientation::VERTICAL)
    , m_article(article)
    , m_color(color)
    , m_price(price)
{
    setup_ui();
}

void ArticleButton::setup_ui()
{
    auto& s = ScaleHelper::instance();

    m_article_font_size = s.pt_str(20);
    m_price_font_size = s.pt_str(18);
    m_amount_font_size = s.pt_str(18);

    static unsigned int s_counter = 0;
    m_css_id = "article-tile-" + std::to_string(++s_counter);
    set_name(m_css_id);
    add_css_class("article-tile");
    add_css_class(m_css_id);

    set_size_request(s.px(130), s.px(90));
    set_hexpand(true);
    set_vexpand(true);
    set_spacing(s.px(6));
    set_margin_start(s.px(2));
    set_margin_end(s.px(2));
    set_margin_top(s.px(2));
    set_margin_bottom(s.px(2));

    // 1. Kopfbereich: Artikelname steht für sich alleine
    m_header_box.set_can_target(false);
    m_header_box.set_valign(Gtk::Align::START);
    m_header_box.set_hexpand(true);

    m_article_label.add_css_class("article-title");
    m_article_label.set_halign(Gtk::Align::CENTER);
    m_article_label.set_valign(Gtk::Align::CENTER);
    m_article_label.set_hexpand(true);
    m_article_label.set_wrap(true);
    m_article_label.set_justify(Gtk::Justification::CENTER);
    m_article_label.set_can_target(false);
    m_header_box.append(m_article_label);

    append(m_header_box);

    // 2. Button-Bereich: Touch-freundlicher Hauptbutton (+) und Dekrement-Button (-)
    m_button_box.add_css_class("button-container");
    m_button_box.set_spacing(s.px(24));
    m_button_box.set_halign(Gtk::Align::CENTER);
    m_button_box.set_valign(Gtk::Align::CENTER);
    m_button_box.set_hexpand(true);
    m_button_box.set_vexpand(true);

    // Haupt-Inkrement-Button (+) - touch-optimiert
    m_btn_plus.add_css_class("btn-plus");
    m_btn_plus.set_hexpand(false);
    m_btn_plus.set_vexpand(false);
    m_btn_plus.set_halign(Gtk::Align::CENTER);
    m_btn_plus.set_valign(Gtk::Align::END);
    m_btn_plus.set_size_request(s.px(120), s.px(120));
    m_lbl_plus.set_can_target(false);
    m_lbl_plus.set_halign(Gtk::Align::CENTER);
    m_lbl_plus.set_valign(Gtk::Align::CENTER);
    m_btn_plus.set_child(m_lbl_plus);
    m_btn_plus.signal_clicked().connect(sigc::mem_fun(*this, &ArticleButton::on_plus_clicked));
    m_button_box.append(m_btn_plus);

    // Dekrement-Button (-) - touch-optimiert
    m_btn_minus.add_css_class("btn-minus");
    m_btn_minus.set_hexpand(false);
    m_btn_minus.set_vexpand(false);
    m_btn_minus.set_halign(Gtk::Align::CENTER);
    m_btn_minus.set_valign(Gtk::Align::END);
    m_btn_minus.set_size_request(s.px(75), s.px(75));
    m_lbl_minus.set_can_target(false);
    m_lbl_minus.set_halign(Gtk::Align::CENTER);
    m_lbl_minus.set_valign(Gtk::Align::CENTER);
    m_btn_minus.set_child(m_lbl_minus);
    m_btn_minus.signal_clicked().connect(sigc::mem_fun(*this, &ArticleButton::on_minus_clicked));
    m_button_box.append(m_btn_minus);

    append(m_button_box);

    // 3. Fußbereich (unterhalb der Buttons): Klickzähler-Badge links, Einzelpreis rechts
    m_footer_box.set_spacing(s.px(4));
    m_footer_box.set_can_target(false);
    m_footer_box.set_valign(Gtk::Align::END);
    m_footer_box.set_hexpand(true);

    // Klickzähler-Badge
    m_amount_label.add_css_class("article-badge");
    m_amount_label.set_halign(Gtk::Align::START);
    m_amount_label.set_valign(Gtk::Align::CENTER);
    m_amount_label.set_can_target(false);
    m_footer_box.append(m_amount_label);

    // Platzhalter für zentrierte Ausrichtung bzw. Auseinanderschieben
    auto placeholder = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    placeholder->set_hexpand(true);
    m_footer_box.append(*placeholder);

    // Einzelpreis
    m_price_label.add_css_class("article-price");
    m_price_label.set_halign(Gtk::Align::END);
    m_price_label.set_valign(Gtk::Align::CENTER);
    m_price_label.set_can_target(false);
    m_footer_box.append(m_price_label);

    append(m_footer_box);

    // CSS Provider
    m_css_provider = Gtk::CssProvider::create();
    get_style_context()->add_provider(m_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    m_button_box.get_style_context()->add_provider(m_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    m_btn_plus.get_style_context()->add_provider(m_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    m_btn_minus.get_style_context()->add_provider(m_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    m_amount_label.get_style_context()->add_provider(m_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

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

void ArticleButton::set_amount(int amount)
{
    m_amount = std::max(0, amount);
    update_display();
}

double ArticleButton::get_total_price() const
{
    return static_cast<double>(m_amount) * m_price;
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

sigc::signal<void()>& ArticleButton::signal_clicked()
{
    return m_signal_clicked;
}

void ArticleButton::on_plus_clicked()
{
    m_amount++;
    update_display();
    m_signal_clicked.emit();
}

void ArticleButton::on_minus_clicked()
{
    if (m_amount > 0) {
        m_amount--;
        update_display();
        m_signal_clicked.emit();
    }
}

bool ArticleButton::is_light_color(const std::string& hex)
{
    if (hex.empty()) return false;
    Gdk::RGBA rgba;
    if (rgba.set(hex)) {
        double luminance = 0.299 * rgba.get_red() + 0.587 * rgba.get_green() + 0.114 * rgba.get_blue();
        return luminance > 0.55;
    }
    return false;
}

std::string ArticleButton::lighten_color(const std::string& hex, double factor)
{
    if (hex.empty()) return "";
    Gdk::RGBA rgba;
    if (!rgba.set(hex)) return hex;
    int r = std::clamp(static_cast<int>(std::round((rgba.get_red() + (1.0 - rgba.get_red()) * factor) * 255.0)), 0, 255);
    int g = std::clamp(static_cast<int>(std::round((rgba.get_green() + (1.0 - rgba.get_green()) * factor) * 255.0)), 0, 255);
    int b = std::clamp(static_cast<int>(std::round((rgba.get_blue() + (1.0 - rgba.get_blue()) * factor) * 255.0)), 0, 255);
    char buf[10];
    std::snprintf(buf, sizeof(buf), "#%02x%02x%02x", r, g, b);
    return std::string(buf);
}

void ArticleButton::update_display()
{
    auto& s = ScaleHelper::instance();
    const std::string color_plus = !m_color.empty() ? m_color : "#4A90E2";
    const std::string color_minus = lighten_color(color_plus, 0.40);
    const bool light_bg = is_light_color(color_plus);

    // 1. Artikelbeschriftung
    const std::string text_color = light_bg ? "#0f172a" : "#ffffff";
    const std::string text_muted = light_bg ? "#334155" : "#e2e8f0";

    const std::string escaped_name = Glib::Markup::escape_text(m_article);
    m_article_label.set_markup(
        "<span weight='bold' size='" + m_article_font_size + "' foreground='" + text_color + "'>" + escaped_name + "</span>"
    );

    // 2. Einzelpreis
    const std::string escaped_price = Glib::Markup::escape_text(format_currency(m_price, m_currency_symbol));
    m_price_label.set_markup(
        "<span weight='bold' size='" + m_price_font_size + "' foreground='" + text_muted + "'>" + escaped_price + "</span>"
    );

    // 3. Klickzähler-Badge
    if (true) {
        m_amount_label.set_markup(
            "<span weight='bold' size='" + m_amount_font_size + "' foreground='" + text_muted + "'>" + std::to_string(m_amount) + "</span>"
        );
    } else {
        m_amount_label.set_markup(
            "<span weight='bold' size='" + m_amount_font_size + "' foreground='" + text_muted + "'>0</span>"
        );
    }

    // 4. "+" und "−" Beschriftung mit passendem Farbkontrast
    const std::string fg_plus = is_light_color(color_plus) ? "#0f172a" : "#ffffff";
    const std::string fg_minus = is_light_color(color_minus) ? "#0f172a" : "#ffffff";

    m_lbl_plus.set_markup(
        "<span weight='bold' size='" + s.pt_str(30) + "' foreground='" + fg_plus + "'>+</span>"
    );
    m_lbl_minus.set_markup(
        "<span weight='bold' size='" + s.pt_str(24) + "' foreground='" + fg_minus + "'>−</span>"
    );

    // 5. Tooltip
    if (!m_article.empty()) {
        if (m_amount > 0) {
            std::string tip = m_article + ": " + std::to_string(m_amount) + " × " +
                              format_currency(m_price, m_currency_symbol).raw() + " = " +
                              format_currency(get_total_price(), m_currency_symbol).raw();
            set_tooltip_text(tip);
        } else {
            set_tooltip_text(m_article + " (" + format_currency(m_price, m_currency_symbol).raw() + ")");
        }
    } else {
        set_tooltip_text("");
    }

    update_style();
}

void ArticleButton::update_style()
{
    auto& s = ScaleHelper::instance();
    const std::string color_plus = !m_color.empty() ? m_color : "#4A90E2";
    const std::string color_minus = lighten_color(color_plus, 0.40);

    std::ostringstream css;

    // Kachel-Container: Hintergrundfarbe bis zum äußeren Rahmen des ArticleButton
    css << "#" << m_css_id << ", ." << m_css_id << " {\n"
        << "    background-color: " << color_plus << ";\n"
        << "    border-radius: " << s.px(12) << "px;\n"
        << "    padding: " << s.px(8) << "px " << s.px(10) << "px;\n"
        << "    border: 1px solid rgba(0, 0, 0, 0.20);\n"
        << "    box-shadow: 0 " << s.px(3) << "px " << s.px(6) << "px rgba(0, 0, 0, 0.28);\n"
        << "}\n";

    // Frame/Container um die beiden Buttons (+ und -)
    css << "#" << m_css_id << " .button-container, .button-container {\n"
        << "    background-color: transparent;\n"
        << "    padding: 0;\n"
        << "}\n";

    // Haupt-Inkrement-Button (+) - quadratisch
    css << "#" << m_css_id << " .btn-plus, .btn-plus {\n"
        << "    background-color: " << color_plus << ";\n"
        << "    background-image: none;\n"
        << "    padding: 0;\n"
        << "    border-radius: " << s.px(10) << "px;\n"
        << "    border: 1px solid rgba(0, 0, 0, 0.25);\n"
        << "    box-shadow: 0 " << s.px(3) << "px " << s.px(5) << "px rgba(0, 0, 0, 0.32);\n"
        << "    transition: all 100ms ease;\n"
        << "}\n"
        << "#" << m_css_id << " .btn-plus:hover, .btn-plus:hover {\n"
        << "    filter: brightness(1.10);\n"
        << "    box-shadow: 0 " << s.px(4) << "px " << s.px(8) << "px rgba(0, 0, 0, 0.40);\n"
        << "}\n"
        << "#" << m_css_id << " .btn-plus:active, .btn-plus:active {\n"
        << "    transform: translateY(" << s.px(2) << "px);\n"
        << "    filter: brightness(0.92);\n"
        << "    box-shadow: 0 " << s.px(1) << "px " << s.px(2) << "px rgba(0, 0, 0, 0.22);\n"
        << "}\n";

    // Dezent aufgehellter Dekrement-Button (-) - quadratisch
    css << "#" << m_css_id << " .btn-minus, .btn-minus {\n"
        << "    background-color: " << color_minus << ";\n"
        << "    background-image: none;\n"
        << "    padding: 0;\n"
        << "    border-radius: " << s.px(10) << "px;\n"
        << "    border: 1px solid rgba(0, 0, 0, 0.18);\n"
        << "    box-shadow: 0 " << s.px(2) << "px " << s.px(4) << "px rgba(0, 0, 0, 0.22);\n"
        << "    transition: all 100ms ease;\n"
        << "}\n"
        << "#" << m_css_id << " .btn-minus:hover, .btn-minus:hover {\n"
        << "    filter: brightness(1.08);\n"
        << "    box-shadow: 0 " << s.px(3) << "px " << s.px(6) << "px rgba(0, 0, 0, 0.28);\n"
        << "}\n"
        << "#" << m_css_id << " .btn-minus:active, .btn-minus:active {\n"
        << "    transform: translateY(" << s.px(2) << "px);\n"
        << "    filter: brightness(0.92);\n"
        << "    box-shadow: 0 " << s.px(1) << "px " << s.px(2) << "px rgba(0, 0, 0, 0.15);\n"
        << "}\n";

    css << "#" << m_css_id << " .article-badge, .article-badge {\n"
        << "    border-radius: " << s.px(8) << "px;\n"
        << "    padding: " << s.px(1) << "px " << s.px(6) << "px;\n"
        << "    background-color: " << "rgba(255, 255, 255, 0.22)" << ";\n"
        << "    color: " << "#ffffff" << ";\n"
        << "}\n";

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
