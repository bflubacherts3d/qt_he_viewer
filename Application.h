#pragma once

#include <A3DSDKIncludes.h>
#include <QtWidgets/QtWidgets>
#include "hps.h"

class MainWindow;

class Application : public QApplication {
    Q_OBJECT;
public:
    Application(int &argc, char **argv);
    virtual ~Application(void);
  
private:
    A3DAsmModelFile *_model_file = nullptr;
public:
    A3DAsmModelFile *getModelFile(void) const {
        return _model_file;
    }
    void setModelFile(A3DAsmModelFile *model_file) {
        if(nullptr != _model_file) {
            A3DAsmModelFileDelete(_model_file);
        }
        _model_file = model_file;
    }

private:
    HPS::World _world;
    QPointer<MainWindow> _main_window;
    
};
