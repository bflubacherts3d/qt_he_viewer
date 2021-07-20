#include "MainWindow.h"
#include "hoops_license.h"

#include "Application.h"

Application::Application(int &argc, char **argv)
: QApplication(argc, argv),
_world(HOOPS_LICENSE) {
    // Initialize HOOPS Exchange
    
    _main_window = new MainWindow();
    _main_window->show();
}

Application::~Application(void) {
    
}
