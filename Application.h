#pragma once

#include <QtWidgets/QtWidgets>

class MainWindow;

class Application : public QApplication {
    Q_OBJECT;
public:
    Application(int &argc, char **argv);
    virtual ~Application(void);
  
private:
    QPointer<MainWindow> _main_window;
};
