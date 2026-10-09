#pragma once

#include <string>
#include <vector>

/**
 * @brief Konfigurationsstruktur für einen Artikel im POS-System.
 *
 * Kann aus einer YAML-Datei geladen werden:
 * - artikelname / name: Anzeigename des Artikels
 * - artikelpreis / preis / price: Einzelpreis in Euro
 * - farbe / color: Hex-Farbcode (z.B. "#8B5A2B")
 * - sichtbarkeit / visible: Ob der Artikel angezeigt wird (true/false)
 * - position (x, y): Position in der 4x5 Matrix (x: 0..3 Spalte, y: 0..4 Zeile)
 */
struct ArticleConfig {
    std::string name;             ///< Name des Artikels
    double price{0.0};            ///< Preis in Euro
    std::string color{"#4A90E2"}; ///< Hex-Farbwert
    bool visible{true};           ///< Sichtbarkeit
    int x{0};                     ///< Spalte (0..3)
    int y{0};                     ///< Zeile (0..4)
};

/**
 * @brief Lädt Artikel aus einer YAML-Datei.
 * Falls die Datei nicht existiert, wird automatisch eine Standarddatei mit 20 Artikeln
 * für die 4x5 Matrix erstellt.
 *
 * @param filepath Pfad zur YAML-Datei (z.B. "articles.yaml")
 * @return std::vector<ArticleConfig> Liste der geladenen Artikel
 */
std::vector<ArticleConfig> load_articles_from_yaml(const std::string& filepath);

/**
 * @brief Erstellt eine Standard-YAML-Datei mit einem realistischen 4x5 Sortiment.
 *
 * @param filepath Pfad zur Zieldatei
 * @return true bei Erfolg, false bei Schreibfehler
 */
bool create_default_articles_yaml(const std::string& filepath);

