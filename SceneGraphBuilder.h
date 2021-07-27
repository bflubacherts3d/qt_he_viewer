#pragma once

#include <QtCore/QtCore>
#include <hps.h>
#include "A3DSDKIncludes.h"
#include "ExchangeToolkit.h"

class SceneGraphBuilder {
public:
    SceneGraphBuilder( A3DAsmModelFile *model_file );
    
    HPS::SegmentKey build(void);

private:
    struct GeometryKits {
        QVector<HPS::ShellKit> shell_kits;
        QVector<HPS::LineKit>  line_kits;
    };
    
    static QVector<HPS::ShellKit> createShellKits(A3DRiRepresentationItem *rep_item);
    static QVector<HPS::LineKit>  createLineKits(A3DRiRepresentationItem *rep_item);
    static HPS::MatrixKit computeNetTransform(ts3d::InstancePath const &instance_path);
    static HPS::MaterialKit createMaterialKit(A3DGraphStyleData const &style_data);

    A3DAsmModelFile *_model_file;
};
