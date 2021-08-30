#include "BOMTableWidget.h"
#include "Application.h"

BOMTableWidget::BOMTableWidget(QWidget *parent) : QTableWidget( parent ) {
    setColumnCount(2);
    refresh();
}

void BOMTableWidget::refresh() {
    clear();
    setHorizontalHeaderLabels({ "Part Name", "#" });
    setRowCount(0);
}
