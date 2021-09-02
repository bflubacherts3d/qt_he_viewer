#pragma once

#include <QtWidgets/QtWidgets>
#include "HPSWidget.h"
#include "BOMTableWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT;
public:
    MainWindow();
    virtual ~MainWindow();
    
protected:
    virtual void closeEvent(QCloseEvent* event) override;

private slots:
    void onFileOpen();
    void onFileClose();
    
private:
    QPointer<ts3d::HPSWidget> _hps_widget;
    QPointer<BOMTableWidget> _bom_table_widget;
};
