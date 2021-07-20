#include "BOMTableWidget.h"

BOMTableWidget::BOMTableWidget(QWidget *parent) : QTableWidget( parent ) {
    setColumnCount(2);
    setHorizontalHeaderLabels({ "Part Name", "#" });
}
