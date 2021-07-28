#include <QtCore/QtCore>
#include "SceneGraphBuilder.h"

SceneGraphBuilder::SceneGraphBuilder(A3DAsmModelFile *model_file)
: _model_file(model_file) {
    
}

HPS::SegmentKey SceneGraphBuilder::build() {
    if(nullptr == _model_file) {
        return HPS::SegmentKey();
    }
    
    // Create a root segment corresponding to the A3DAsmModelFile
    auto root_key = HPS::Database::CreateRootSegment();

    return root_key;
}

// static
QVector<HPS::ShellKit> SceneGraphBuilder::createShellKits(A3DRiRepresentationItem *rep_item) {
    QVector<HPS::ShellKit> shell_kits;

    return shell_kits;
}

// static
QVector<HPS::LineKit>  SceneGraphBuilder::createLineKits(A3DRiRepresentationItem *rep_item) {
    QVector<HPS::LineKit> line_kits;

    return line_kits;
}

namespace {
    HPS::Vector toHPSVector(A3DVector3dData const &v) {
        return HPS::Vector(static_cast<float>(v.m_dX), static_cast<float>(v.m_dY), static_cast<float>(v.m_dZ));
    }
}

// static
HPS::MatrixKit SceneGraphBuilder::computeNetTransform(std::vector<A3DEntity*> const &instance_path) {
    HPS::MatrixKit net_martix;

    return net_martix;
}


HPS::MaterialKit SceneGraphBuilder::createMaterialKit( A3DGraphStyleData const &style_data ) {
    HPS::MaterialKit mk;
    return mk;
}
