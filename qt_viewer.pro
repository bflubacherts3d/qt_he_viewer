include(hoops_config.pri)

TEMPLATE = app
CONFIG -= release debug_and_release qt
CONFIG *= qt c++14 exceptions rtti debug
win32 {
    CONFIG *= console
}

QT *= gui widgets
TARGET = qt_viewer

INCLUDEPATH += . \
    $${HOOPS_VISUALIZE_PATH}/include \
    $${HOOPS_EXCHANGE_PATH}/include

HEADERS += Application.h \
    BOMTableWidget.h \
    MainWindow.h \
    HPSWidget.h \
    SceneGraphBuilder.h \


SOURCES += Application.cpp \
    BOMTableWidget.cpp\
    main.cpp \
    MainWindow.cpp \
    HPSWidget.cpp \
    SceneGraphBuilder.cpp \

macx {
    LIBS += -L$${HOOPS_VISUALIZE_PATH}/bin/macos 
    LIBS += -lhps_sprk_ops -lhps_sprk -lhps_core
    LIBS += -Wl,-rpath,$${HOOPS_VISUALIZE_PATH}/bin/macos
}
win32 {
    release:D=
    debug:D=d
    lib_path = $${HOOPS_VISUALIZE_PATH}/lib/win64_v141$${D}
    LIBS += $${lib_path}/hps_core.lib \
            $${lib_path}/hps_sprk.lib \
            $${lib_path}/hps_sprk_ops.lib
}
