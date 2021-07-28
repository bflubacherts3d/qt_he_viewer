#include "Application.h"
#include "MainWindow.h"
#include "hoops_license.h"


Application::Application(int &argc, char **argv)
: QApplication(argc, argv),
_world(HOOPS_LICENSE) {
    
    setApplicationName("Tutorial");
    setApplicationDisplayName("HOOPS Exchange Tutorial");
    
    // Initialize HOOPS Exchange

    
    _main_window = new MainWindow();
    _main_window->show();
}

Application::~Application(void) {
    // Terminate HOPOS Exchange
}
