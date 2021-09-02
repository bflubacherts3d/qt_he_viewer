#include "Application.h"
#include "MainWindow.h"
#include "hoops_license.h"


Application::Application(int &argc, char **argv)
: QApplication(argc, argv),
_world(HOOPS_LICENSE) {
    
    setApplicationName("Tutorial");
    setApplicationDisplayName("HOOPS Exchange Tutorial");
    setOrganizationDomain("techsoft3d.com");
    setOrganizationName("TechSoft 3D");
    
    // Initialize HOOPS Exchange

    
    _main_window = new MainWindow();
    _main_window->show();
}

Application::~Application(void) {
    // Terminate HOPOS Exchange
}
