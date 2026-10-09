#include "article_config.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>

namespace {

std::string trim(const std::string& s) {
    const auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    const auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string strip_quotes(std::string s) {
    s = trim(s);
    if (s.size() >= 2 && ((s.front() == '"' && s.back() == '"') || (s.front() == '\'' && s.back() == '\''))) {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}

bool parse_bool(const std::string& s) {
    const std::string lower = to_lower(trim(s));
    return (lower == "true" || lower == "1" || lower == "yes" || lower == "ja" || lower == "wahr");
}

bool is_hex_color(const std::string& s, size_t pos) {
    if (pos >= s.size() || s[pos] != '#') return false;
    size_t len = 0;
    for (size_t i = pos + 1; i < s.size(); ++i) {
        if (std::isxdigit(static_cast<unsigned char>(s[i]))) {
            len++;
        } else {
            break;
        }
    }
    return (len == 3 || len == 4 || len == 6 || len == 8);
}

std::string strip_comments(const std::string& line) {
    bool in_double_quote = false;
    bool in_single_quote = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"' && !in_single_quote) {
            in_double_quote = !in_double_quote;
        } else if (c == '\'' && !in_double_quote) {
            in_single_quote = !in_single_quote;
        } else if (c == '#' && !in_double_quote && !in_single_quote) {
            if (is_hex_color(line, i)) {
                // Hex-Farbcode (z.B. #8B5A2B) nicht als Kommentar entfernen
                continue;
            }
            if (i == 0 || std::isspace(static_cast<unsigned char>(line[i - 1]))) {
                return line.substr(0, i);
            }
        }
    }
    return line;
}

void parse_position_string(const std::string& val, int& x, int& y) {
    std::string s = to_lower(val);
    const auto x_pos = s.find("x:");
    const auto y_pos = s.find("y:");
    if (x_pos != std::string::npos && y_pos != std::string::npos) {
        try {
            auto x_str = s.substr(x_pos + 2);
            auto comma = x_str.find(',');
            if (comma != std::string::npos) x_str = x_str.substr(0, comma);
            x = std::stoi(trim(x_str));

            auto y_str = s.substr(y_pos + 2);
            auto close_brace = y_str.find('}');
            if (close_brace != std::string::npos) y_str = y_str.substr(0, close_brace);
            y = std::stoi(trim(y_str));
            return;
        } catch (...) {}
    }

    std::string cleaned;
    for (char c : val) {
        if (c != '[' && c != ']' && c != '(' && c != ')' && c != '{' && c != '}') {
            cleaned += c;
        }
    }
    const auto comma = cleaned.find(',');
    if (comma != std::string::npos) {
        try {
            x = std::stoi(trim(cleaned.substr(0, comma)));
            y = std::stoi(trim(cleaned.substr(comma + 1)));
        } catch (...) {}
    }
}

} // namespace

bool create_default_articles_yaml(const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out.is_open()) {
        std::cerr << "Fehler beim Erstellen der Datei " << filepath << std::endl;
        return false;
    }

    out << "# ============================================================\n"
        << "# Artikelkonfiguration für das Kassensystem (caisse)\n"
        << "# Matrix-Layout: 4 Spalten (x: 0..3) × 5 Reihen (y: 0..4)\n"
        << "# \n"
        << "# Felder pro Artikel:\n"
        << "#   artikelname:  Name des Artikels (z.B. \"Kaffee\")\n"
        << "#   artikelpreis: Preis in Euro (z.B. 2.50 oder 2,50)\n"
        << "#   farbe:        Hex-Farbwert (z.B. \"#8B5A2B\")\n"
        << "#   sichtbarkeit: true oder false\n"
        << "#   position:     [x, y]  oder x: ..., y: ...\n"
        << "# ============================================================\n\n"
        << "articles:\n";

    // Standard 4x5 Matrix (20 Artikel)
    struct DefItem {
        const char* name;
        double price;
        const char* color;
        bool visible;
        int x;
        int y;
    };

    const DefItem defaults[20] = {
        // Spalte 0: Kaffee & Heißgetränke
        {"Espresso",        2.00, "#6F4E37", true, 0, 0},
        {"Kaffee",          2.50, "#8B5A2B", true, 0, 1},
        {"Cappuccino",      3.20, "#A0522D", true, 0, 2},
        {"Latte Macchiato", 3.60, "#C49A6C", true, 0, 3},
        {"Tee",             2.20, "#2E8B57", true, 0, 4},

        // Spalte 1: Kaltgetränke & Softdrinks
        {"Wasser Still",    1.80, "#2980B9", true, 1, 0},
        {"Wasser Medium",   1.80, "#3498DB", true, 1, 1},
        {"Cola",            2.80, "#C0392B", true, 1, 2},
        {"Limonade",        2.80, "#E67E22", true, 1, 3},
        {"Apfelschorle",    3.00, "#27AE60", true, 1, 4},

        // Spalte 2: Snacks & Herzhaftes
        {"Butterbrezel",    1.60, "#D35400", true, 2, 0},
        {"Croissant",       1.90, "#E59866", true, 2, 1},
        {"Käsebrötchen",    3.20, "#F39C12", true, 2, 2},
        {"Schinkenbrötchen",3.50, "#D98880", true, 2, 3},
        {"Sandwich",        3.80, "#B9770E", true, 2, 4},

        // Spalte 3: Kuchen & Süßes
        {"Apfelkuchen",     3.40, "#A93226", true, 3, 0},
        {"Käsekuchen",      3.50, "#F5B041", true, 3, 1},
        {"Schokomuffin",    2.60, "#4A235A", true, 3, 2},
        {"Donut",           2.20, "#AF7AC5", true, 3, 3},
        {"Schokoriegel",    1.50, "#7D6608", true, 3, 4}
    };

    for (const auto& it : defaults) {
        out << "  - artikelname: \"" << it.name << "\"\n"
            << "    artikelpreis: " << it.price << "\n"
            << "    farbe: \"" << it.color << "\"\n"
            << "    sichtbarkeit: " << (it.visible ? "true" : "false") << "\n"
            << "    position: [" << it.x << ", " << it.y << "]\n\n";
    }

    out.close();
    std::cout << "Standardkonfiguration erfolgreich erstellt: " << filepath << std::endl;
    return true;
}

std::vector<ArticleConfig> load_articles_from_yaml(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "Konfigurationsdatei '" << filepath << "' nicht gefunden. Erstelle Standarddatei..." << std::endl;
        create_default_articles_yaml(filepath);
        file.open(filepath);
        if (!file.is_open()) {
            std::cerr << "Konnte Datei '" << filepath << "' nicht öffnen!" << std::endl;
            return {};
        }
    }

    std::vector<ArticleConfig> articles;
    std::string line;

    ArticleConfig current;
    bool in_item = false;
    bool in_position_block = false;

    auto finish_current = [&]() {
        if (in_item) {
            if (!current.name.empty() || current.price > 0.0) {
                // Begrenze Koordinaten auf die 4x5 Matrix
                current.x = std::clamp(current.x, 0, 3);
                current.y = std::clamp(current.y, 0, 4);
                articles.push_back(current);
            }
            current = ArticleConfig{};
            in_item = false;
            in_position_block = false;
        }
    };

    while (std::getline(file, line)) {
        line = strip_comments(line);
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;

        // Neues Listenelement (beginnt mit '-')
        if (trimmed.rfind("-", 0) == 0) {
            finish_current();
            in_item = true;
            in_position_block = false;
            trimmed = trim(trimmed.substr(1));
            if (trimmed.empty()) {
                continue;
            }
        }

        if (!in_item) {
            continue;
        }

        const auto colon_pos = trimmed.find(':');
        if (colon_pos == std::string::npos) continue;

        const std::string key = to_lower(trim(trimmed.substr(0, colon_pos)));
        const std::string val = trim(trimmed.substr(colon_pos + 1));

        if (key == "position") {
            if (val.empty()) {
                in_position_block = true;
            } else {
                in_position_block = false;
                parse_position_string(val, current.x, current.y);
            }
            continue;
        }

        // Schlüssel-Wert-Zuordnung (sowohl deutsche als auch englische Bezeichnungen unterstützt)
        if (key == "x" || key == "col" || key == "spalte") {
            try { current.x = std::stoi(val); } catch (...) {}
            continue;
        } else if (key == "y" || key == "row" || key == "zeile" || key == "reihe") {
            try { current.y = std::stoi(val); } catch (...) {}
            continue;
        } else {
            // Sobald ein anderer Schlüssel folgt, ist der Position-Unterblock beendet
            in_position_block = false;
        }

        if (key == "artikelname" || key == "name" || key == "titel" || key == "bezeichnung") {
            current.name = strip_quotes(val);
        } else if (key == "artikelpreis" || key == "preis" || key == "price") {
            std::string price_str = strip_quotes(val);
            std::replace(price_str.begin(), price_str.end(), ',', '.');
            try { current.price = std::stod(price_str); } catch (...) {}
        } else if (key == "farbe" || key == "color" || key == "colour") {
            current.color = strip_quotes(val);
        } else if (key == "sichtbarkeit" || key == "sichtbar" || key == "visible" || key == "visibility") {
            current.visible = parse_bool(val);
        }
    }

    finish_current();
    return articles;
}

