HOOPS_VISUALIZE_PATH=/opt/local/ts3d/HOOPS_Visualize_2021_SP1_U1
HOOPS_EXCHANGE_PATH=/opt/local/ts3d/HOOPS_Exchange_2021_SP1_U1
TEMPLATE = app
CONFIG -= release debug_and_release qt
CONFIG *= qt c++14 exceptions rtti debug

QT *= gui widgets
TARGET = qt_viewer

INCLUDEPATH += . \
    $${HOOPS_VISUALIZE_PATH}/include \
    $${HOOPS_EXCHANGE_PATH}/include

HEADERS += Application.h \
    BOMTableWidget.h \
    MainWindow.h \
    HPSWidget.h \
    SceneGraph.h \
    SceneGraphBuilder.h \


SOURCES += Application.cpp \
    BOMTableWidget.cpp\
    main.cpp \
    MainWindow.cpp \
    HPSWidget.cpp \
    SceneGraph.cpp \
    SceneGraphBuilder.cpp \

macx {
    LIBS += -L$${HOOPS_VISUALIZE_PATH}/bin/macos 
    LIBS += -lhps_sprk_ops -lhps_sprk -lhps_core
    LIBS += -Wl,-rpath,$${HOOPS_VISUALIZE_PATH}/bin/macos
}
