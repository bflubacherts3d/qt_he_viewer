#include "BOMTableWidget.h"
#include "Application.h"
#include <A3DSDKIncludes.h>
#include <ExchangeToolkit.h>

BOMTableWidget::BOMTableWidget(QWidget *parent) : QTableWidget( parent ) {
    setColumnCount(2);
    refresh();
}

void BOMTableWidget::refresh() {
    clear();
    setHorizontalHeaderLabels({ "Part Name", "#" });

    auto const model_file = static_cast<Application*>(qApp)->getModelFile();
    if(nullptr == model_file) {
        return;
    }
    
    ts3d::InstancePathMap instance_path_map;
    auto const unique_parts = ts3d::getUniqueChildren(model_file, kA3DTypeAsmPartDefinition, instance_path_map);
    setRowCount(unique_parts.size());
    auto row = 0;
    for(auto const part : unique_parts) {
        auto const &part_instance_paths = instance_path_map[part];
        auto const number_of_instances = part_instance_paths.size();
        auto instance_count_item = new QTableWidgetItem(QString("%1").arg(number_of_instances));
        setItem(row, 1, instance_count_item );

        auto const part_name = ts3d::getName(part);
        if(part_name.empty()) {
            auto an_instance_path = part_instance_paths.front();
            an_instance_path.pop_back();
            const_cast<std::string&>(part_name) = ts3d::getName(an_instance_path.back());
        }
        auto part_name_item = new QTableWidgetItem(part_name.c_str());
        setItem(row, 0, part_name_item);
        row++;
    }
}
