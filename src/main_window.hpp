#pragma once

#include "article_button.hpp"
#include <gtkmm.h>
#include <vector>

class MainWindow : public Gtk::Window {
public:
    MainWindow();
    ~MainWindow() override = default;

private:
    void update_total_display();
    void reset_all();

    Gtk::Box m_main_box{Gtk::Orientation::VERTICAL};
    Gtk::Label m_title_label;
    Gtk::Grid m_grid;
    Gtk::Box m_bottom_box{Gtk::Orientation::HORIZONTAL};
    Gtk::Label m_summary_label;
    Gtk::Button m_reset_button{"Alles zurücksetzen"};

    // Artikel-Buttons für die Demonstration
    ArticleButton m_btn_kaffee{"Kaffee", 2.50, "#8B5A2B"};
    ArticleButton m_btn_cappuccino{"Cappuccino", 3.20, "#A0522D"};
    ArticleButton m_btn_tee{"Tee", 2.20, "#2E8B57"};
    ArticleButton m_btn_cola{"Cola", 2.80, "#C0392B"};
    ArticleButton m_btn_wasser{"Wasser", 1.80, "#2980B9"};
    ArticleButton m_btn_kuchen{"Kuchen", 3.50, "#D35400"};

    std::vector<ArticleButton*> m_articles{
        &m_btn_kaffee,
        &m_btn_cappuccino,
        &m_btn_tee,
        &m_btn_cola,
        &m_btn_wasser,
        &m_btn_kuchen
    };
};
