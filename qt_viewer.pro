HOOPS_VISUALIZE_PATH=C://Users//sumit.kulkarni//source//HOOPS_Visualize_2021_SP2_U1_Win_v140
HOOPS_EXCHANGE_PATH=C://Users//sumit.kulkarni//source//HOOPS_Exchange_Publish_2021_SP2_U1_Win_VS2015
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

win32:CONFIG(release, debug|release): LIBS += -L$${HOOPS_VISUALIZE_PATH}/lib/win64_v140/ -lhps_core -lhps_sprk -lhps_sprk_ops
else:win32:CONFIG(debug, debug|release): LIBS += -L$${HOOPS_VISUALIZE_PATH}/lib/win64_v140d/ -lhps_core -lhps_sprk -lhps_sprk_ops

INCLUDEPATH += $${HOOPS_VISUALIZE_PATH}/lib/win64_v140d
DEPENDPATH += $${HOOPS_VISUALIZE_PATH}/lib/win64_v140d

