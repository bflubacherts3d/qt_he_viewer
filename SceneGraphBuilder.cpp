#include <QtCore/QtCore>
#include "SceneGraphBuilder.h"
#include <ExchangeToolkit.h>

SceneGraphBuilder::SceneGraphBuilder(A3DAsmModelFile *model_file)
: _model_file(model_file) {
    
}

HPS::SegmentKey SceneGraphBuilder::build() {
    if(nullptr == _model_file) {
        return HPS::SegmentKey();
    }
    
    ts3d::InstancePathMap instance_path_map;
    QHash<A3DRiRepresentationItem*, GeometryKits> ri_geomery_kits;
    auto const all_rep_items = ts3d::getUniqueLeafEntities(_model_file, kA3DTypeRiRepresentationItem, instance_path_map);
    for(auto const rep_item : all_rep_items) {
        GeometryKits geometry_kits;
        geometry_kits.shell_kits = createShellKits(rep_item);
        geometry_kits.line_kits  = createLineKits(rep_item);
        ri_geomery_kits[rep_item] = geometry_kits;
    }

    
    // Create a root segment corresponding to the A3DAsmModelFile
    auto root_key = HPS::Database::CreateRootSegment();
    for(auto const &instance_path_map_entry : instance_path_map) {
        auto const this_rep_item = instance_path_map_entry.first;
        auto const &geometry_kits_for_this_rep_item = ri_geomery_kits[this_rep_item];
        auto const &instance_paths = instance_path_map_entry.second;
        for(auto const &instance_path : instance_paths) {
            ts3d::RepresentationItemInstance instance(instance_path);
            if(!instance.Instance::getNetShow() || instance.Instance::getNetRemoved()) {
                continue;
            }
            auto instance_segment = root_key.Subsegment();
            
            // set the modeling matrix
            auto const matrix_kit = computeNetTransform(instance_path);
            instance_segment.SetModellingMatrix(matrix_kit);
            
            // set up a segment for the "default" material
            auto ri_segment = instance_segment.Subsegment();
            auto face_idx = 0u;
            for(auto const &shell_kit : geometry_kits_for_this_rep_item.shell_kits) {
                auto const this_face_idx = face_idx++;
                if(!instance.getNetShow(this_face_idx) || instance.getNetRemoved(this_face_idx)) {
                    continue;
                }
                
                ri_segment.InsertShell(shell_kit);
            }
            
            // insert the line kits to represent edges
            for(auto const &line_kit : geometry_kits_for_this_rep_item.line_kits) {
                instance_segment.InsertLine(line_kit);
            }
        }
    }
    return root_key;
}

// static
QVector<HPS::ShellKit> SceneGraphBuilder::createShellKits(A3DRiRepresentationItem *rep_item) {
    QVector<HPS::ShellKit> shell_kits;
    ts3d::RepresentationItemInstance i({rep_item});
    auto tess3d = std::dynamic_pointer_cast<Tess3DInstance>(i.getTessellation());
    if(nullptr == tess3d) {
        return shell_kits;
    }
    
    auto const exchange_coords = tess3d->coords();
    auto const exchange_normals = tess3d->normals();
    for(auto idx = 0u; idx < tess3d->faceSize(); ++idx) {
        HPS::PointArray vertex_positions;
        HPS::VectorArray vertex_normals;
        auto const face_mesh = tess3d->getIndexMeshForFace( idx );
        HPS::IntArray face_list;
        for(auto idx = 0u; idx < face_mesh.vertices().size(); ++idx) {
            if(0 == idx % 3) {
                face_list.push_back(3);
            }
            auto const exchange_pindex = face_mesh.vertices()[idx];
            auto const next_index = vertex_positions.size();
            vertex_positions.emplace_back(HPS::Point(exchange_coords[exchange_pindex],
                                                        exchange_coords[exchange_pindex+1],
                                                        exchange_coords[exchange_pindex+2]));
            face_list.push_back(static_cast<int>(next_index));
            
            auto const exchange_nindex = face_mesh.normals()[idx];
            auto const vertex_normal = HPS::Vector(exchange_normals[exchange_nindex],
                                                   exchange_normals[exchange_nindex+1],
                                                   exchange_normals[exchange_nindex+2]);
            vertex_normals.emplace_back(vertex_normal);
        }
        HPS::ShellKit shell_kit_for_this_face;
        shell_kit_for_this_face.SetPoints(vertex_positions);
        shell_kit_for_this_face.SetVertexNormalsByRange(0, vertex_normals);
        shell_kit_for_this_face.SetFacelist(face_list);
        shell_kits.append(shell_kit_for_this_face);
    }

    return shell_kits;
}

// static
QVector<HPS::LineKit>  SceneGraphBuilder::createLineKits(A3DRiRepresentationItem *rep_item) {
    QVector<HPS::LineKit> line_kits;
    ts3d::RepresentationItemInstance i({rep_item});
    auto tess3d = std::dynamic_pointer_cast<Tess3DInstance>(i.getTessellation());
    if(nullptr == tess3d) {
        return line_kits;
    }
    
    auto const exchange_coords = tess3d->coords();
    for(auto idx = 0u; idx < tess3d->faceSize(); ++idx) {
        auto const face_mesh = tess3d->getIndexMeshForFace( idx );
        for(auto const &loop : face_mesh.loops()) {
            for(auto const &edge : loop._edges) {
                if(!edge._visible) {
                    continue;
                }
                HPS::PointArray line_points;
                for(auto idx = 0u; idx < edge._vertices.size(); ++idx) {
                    auto const exchange_idx = edge._vertices[idx];
                    line_points.emplace_back(HPS::Point(exchange_coords[exchange_idx],
                                                        exchange_coords[exchange_idx+1],
                                                        exchange_coords[exchange_idx+2]));
                }
                HPS::LineKit line_kit;
                line_kit.SetPoints(line_points);
                line_kits.push_back(line_kit);
            }
        }
    }

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
    for(auto const entity : instance_path) {
        HPS::MatrixKit matrix_kit;
        auto const xform = ts3d::getLocation(entity);
        auto const t = ts3d::getEntityType(xform);
        if(t == kA3DTypeMiscCartesianTransformation) {
            ts3d::A3DMiscCartesianTransformationWrapper d(xform);
            auto const mirror = (d->m_ucBehaviour & kA3DTransformationMirror) ? -1 : 1;
            auto const scale  = (d->m_ucBehaviour & (kA3DTransformationScale | kA3DTransformationNonUniformScale))  ? toHPSVector(d->m_sScale) : HPS::Vector(1.f, 1.f, 1.f);
            auto const x      = (d->m_ucBehaviour & kA3DTransformationRotate) ? toHPSVector(d->m_sXVector) : HPS::Vector::XAxis();
            auto const y      = (d->m_ucBehaviour & kA3DTransformationRotate) ? toHPSVector(d->m_sYVector) : HPS::Vector::YAxis();
            auto const z      = x.Cross(y) * mirror;
            auto const o      = (d->m_ucBehaviour & kA3DTransformationTranslate) ? toHPSVector(d->m_sOrigin) : HPS::Vector(0.f, 0.f, 0.f);
            matrix_kit.SetElements({
                x.x * scale.x, x.y * scale.x, x.z * scale.x, 0.f,
                y.x * scale.y, y.y * scale.y, y.z * scale.y, 0.f,
                z.x * scale.z, z.y * scale.z, z.z * scale.z, 0.f,
                o.x,           o.y,           o.z,           1.f
            } );
        } else if(t == kA3DTypeMiscGeneralTransformation) {
            ts3d::A3DMiscGeneralTransformationWrapper d(xform);
            for(auto j = 0u; j < 4u; j++) {
                auto const offset = 4u * j;
                for(auto i = 0u; i < 4u; i++) {
                    matrix_kit.SetElement(i, j, static_cast<float>(d->m_adCoeff[i + offset]));
                }
            }
        }
        net_martix = matrix_kit * net_martix;
    }

    return net_martix;
}


HPS::MaterialKit SceneGraphBuilder::createMaterialKit( A3DGraphStyleData const &style_data ) {
    HPS::MaterialKit mk;
    return mk;
}
