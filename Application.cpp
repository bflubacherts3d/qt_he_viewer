#define INITIALIZE_A3D_API
#include <A3DSDKIncludes.h>

#include "Application.h"
#include "MainWindow.h"
#include "hoops_license.h"


Application::Application(int &argc, char **argv)
: QApplication(argc, argv),
_world(HOOPS_LICENSE) {
    
    setApplicationName("Tutorial");
    setApplicationDisplayName("HOOPS Exchange Tutorial");
    
    // Initialize HOOPS Exchange
    if(!A3DSDKLoadLibrary("/opt/local/ts3d/HOOPS_Exchange_2021_SP1_U1/bin/osx64")) {
        throw std::runtime_error("Unable to load HOOPS Exchange.");
    }

    if(A3D_SUCCESS != A3DLicPutUnifiedLicense(HOOPS_LICENSE)) {
        throw std::runtime_error("Invalid HOOPS Exchange license.");
    }

    if(A3D_SUCCESS != A3DDllInitialize(A3D_DLL_MAJORVERSION, A3D_DLL_MINORVERSION)) {
        throw std::runtime_error("HOOPS Exchange version mismatch.");
    }

    
    _main_window = new MainWindow();
    _main_window->show();
}

Application::~Application(void) {
    // Terminate HOPOS Exchange
    A3DDllTerminate();
    A3DSDKUnloadLibrary();
}
