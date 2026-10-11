#include "sales_logger.hpp"
#include "article_button.hpp"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <algorithm>

std::string SalesLogger::get_current_filename()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_r(&t, &tm);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d_overview_sales.csv", &tm);
    return std::string(buf);
}

bool SalesLogger::log_sale(const std::vector<std::unique_ptr<ArticleButton>>& articles, double total_price)
{
    std::vector<std::pair<std::string, int>> items;
    items.reserve(articles.size());
    for (const auto& btn : articles) {
        if (btn) {
            items.emplace_back(btn->get_article(), btn->get_amount());
        }
    }
    return log_sale(items, total_price);
}

bool SalesLogger::log_sale(const std::vector<std::pair<std::string, int>>& items, double total_price)
{
    const std::string filename = get_current_filename();

    std::error_code ec;
    const bool file_exists = std::filesystem::exists(filename, ec);
    const bool is_empty = file_exists ? (std::filesystem::file_size(filename, ec) == 0) : true;
    const bool write_header = !file_exists || is_empty;

    std::ofstream out(filename, std::ios::app);
    if (!out.is_open()) {
        std::cerr << "[SalesLogger] Fehler beim Öffnen von " << filename << std::endl;
        return false;
    }

    if (write_header) {
        out << "time";
        for (const auto& [name, _] : items) {
            out << ";" << name;
        }
        out << ";total\n";
    }

    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_r(&t, &tm);
    char time_buf[32];
    std::strftime(time_buf, sizeof(time_buf), "%H:%M:%S", &tm);

    out << time_buf;
    for (const auto& [_, count] : items) {
        out << ";" << count;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << total_price;
    std::string total_str = oss.str();
    std::replace(total_str.begin(), total_str.end(), '.', ',');

    out << ";" << total_str << "\n";
    out.flush();

    return true;
}

