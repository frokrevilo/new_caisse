#include "main_window.hpp"
#include <gtkmm/application.h>

int main(int argc, char* argv[])
{
    // Erstelle GTKmm-Applikation mit eindeutiger Application-ID
    auto app = Gtk::Application::create("de.pos.caisse");

    // Startet die Anwendung und zeigt das MainWindow an
    return app->make_window_and_run<MainWindow>(argc, argv);
}
