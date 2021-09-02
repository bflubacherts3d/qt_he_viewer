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
    setOrganizationDomain("techsoft3d.com");
    setOrganizationName("TechSoft 3D");
    
    // Initialize HOOPS Exchange
    QString const exchange_bin_folder = "/opt/local/ts3d/HOOPS_Exchange_2021_SP1_U1/bin/osx64";
#ifdef _UNICODE
    if(!A3DSDKLoadLibrary( exchange_bin_folder.toStdWString().c_str())) {
        throw std::runtime_error("Unable to load HOOPS Exchange.");
    }
#else
    if (!A3DSDKLoadLibrary(exchange_bin_folder.toStdString().c_str())) {
        throw std::runtime_error("Unable to load HOOPS Exchange.");
    }
#endif

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
