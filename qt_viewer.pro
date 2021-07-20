HOOPS_VISUALIZE_PATH=/opt/local/ts3d/HOOPS_Visualize_2021_SP1_U1

TEMPLATE = app
CONFIG *= qt c++14 exceptions rtti
QT *= gui widgets
TARGET = qt_viewer
INCLUDEPATH += . $${HOOPS_VISUALIZE_PATH}/include

HEADERS += Application.h \
    BOMTableWidget.h \
    MainWindow.h \
    HPSWidget.h

SOURCES += Application.cpp \
    BOMTableWidget.cpp\
    main.cpp \
    MainWindow.cpp \
    HPSWidget.cpp

macx {
    LIBS += -L$${HOOPS_VISUALIZE_PATH}/bin/macos 
    LIBS += -lhps_sprk_ops -lhps_sprk -lhps_core
    LIBS += -Wl,-rpath,$${HOOPS_VISUALIZE_PATH}/bin/macos
}
