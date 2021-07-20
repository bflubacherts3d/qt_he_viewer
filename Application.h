#pragma once

#include <QtWidgets/QtWidgets>
#include "hps.h"

class MainWindow;

class Application : public QApplication {
    Q_OBJECT;
public:
    Application(int &argc, char **argv);
    virtual ~Application(void);
  
private:
    HPS::World _world;
    QPointer<MainWindow> _main_window;
};
