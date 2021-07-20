#include "MainWindow.h"
#include "hps.h"
#include "hoops_license.h"

#include "Application.h"

Application::Application(int &argc, char **argv) : QApplication(argc, argv) {

    // Initialize HOOPS Visualize
    HPS::World world( HOOPS_LICENSE );

    // Initialize HOOPS Exchange
    
    _main_window = new MainWindow();
    _main_window->show();
}

Application::~Application(void) {
    
}
