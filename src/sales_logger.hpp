#pragma once

#include <string>
#include <vector>
#include <memory>
#include <utility>

class ArticleButton;

/**
 * @brief Verwaltet die Protokollierung aller getätigten Verkäufe in eine tägliche CSV-Datei.
 * 
 * Dateinamenformat: yyyy-mm-dd_overview_sales.csv
 * Spalten: time;article_name_1;...;article_name_20;total
 * Trennzeichen: ';'
 */
class SalesLogger {
public:
    /**
     * @brief Speichert einen Verkauf anhand der aktuellen Artikel-Buttons und des Gesamtbetrags.
     * 
     * @param articles Liste aller 20 Artikel-Buttons der Matrix
     * @param total_price Gesamtsumme des Verkaufs in Euro
     * @return true bei erfolgreichem Schreiben, false bei Fehler
     */
    static bool log_sale(const std::vector<std::unique_ptr<ArticleButton>>& articles, double total_price);

    /**
     * @brief Speichert einen Verkauf anhand einer Liste von Artikelnamen und Mengen.
     * 
     * @param items Liste von Paaren (Artikelname, verkaufte Stückzahl)
     * @param total_price Gesamtsumme des Verkaufs in Euro
     * @return true bei erfolgreichem Schreiben, false bei Fehler
     */
    static bool log_sale(const std::vector<std::pair<std::string, int>>& items, double total_price);

    /// Liefert den heutigen CSV-Dateinamen im Format "yyyy-mm-dd_overview_sales.csv"
    static std::string get_current_filename();
};

