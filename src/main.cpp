#include "main_window.hpp"
#include <gtkmm/application.h>

#include <clocale>

int main(int argc, char* argv[])
{
    // Erstelle GTKmm-Applikation mit eindeutiger Application-ID
    auto app = Gtk::Application::create("de.pos.caisse");

    // Standard-Dezimalpunkt (.) für C/C++ Bibliotheksfunktionen sicherstellen
    std::setlocale(LC_NUMERIC, "C");

    // Startet die Anwendung und zeigt das MainWindow an
    return app->make_window_and_run<MainWindow>(argc, argv);
}
