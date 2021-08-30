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
            auto const ri_net_style = instance.Instance::getNetStyle();
            auto const ri_material_kit = createMaterialKit(ri_net_style);
            ri_segment.GetMaterialMappingControl().SetFaceMaterial(ri_material_kit);
            
            QVector<QPair<A3DGraphStyleData,HPS::SegmentKey>> unique_face_style_subsegments;
            auto face_idx = 0u;
            for(auto const &shell_kit : geometry_kits_for_this_rep_item.shell_kits) {
                auto const this_face_idx = face_idx++;
                if(!instance.getNetShow(this_face_idx) || instance.getNetRemoved(this_face_idx)) {
                    continue;
                }
                
                auto const face_net_style = instance.getNetStyle(this_face_idx);
                if(0 != memcmp(&face_net_style, &ri_net_style, sizeof(A3DGraphStyleData))) {
                    // the style for this particular face differs from the owning body style
                    auto found_existing_subsegment = false;
                    for(auto &unique_face_style_subsegment: unique_face_style_subsegments) {
                        if(0 == memcmp(&face_net_style, &unique_face_style_subsegment.first, sizeof(A3DGraphStyleData))) {
                            unique_face_style_subsegment.second.InsertShell(shell_kit);
                            found_existing_subsegment = true;
                            break;
                        }
                    }
                    if(!found_existing_subsegment) {
                        auto unique_face_style_subsegment = ri_segment.Subsegment();
                        unique_face_style_subsegment.GetMaterialMappingControl().SetFaceMaterial(createMaterialKit(face_net_style));
                        unique_face_style_subsegment.InsertShell(shell_kit);
                        unique_face_style_subsegments.push_back(qMakePair(face_net_style, unique_face_style_subsegment));
                    }
                } else {
                    ri_segment.InsertShell(shell_kit);
                }
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
        QHash<A3DUns32, size_t> exchange_to_hps_index_map;
        HPS::IntArray face_list;
        for(auto idx = 0u; idx < face_mesh.vertices().size(); ++idx) {
            if(0 == idx % 3) {
                face_list.push_back(3);
            }
            auto const exchange_pindex = face_mesh.vertices()[idx];
            auto const it = exchange_to_hps_index_map.find(exchange_pindex);
            if( std::end(exchange_to_hps_index_map) == it ) {
                auto const next_index = vertex_positions.size();
                vertex_positions.emplace_back(HPS::Point(exchange_coords[exchange_pindex],
                                                         exchange_coords[exchange_pindex+1],
                                                         exchange_coords[exchange_pindex+2]));
                exchange_to_hps_index_map[exchange_pindex] = next_index;
                face_list.push_back(static_cast<int>(next_index));
            } else {
                face_list.push_back(static_cast<int>(it.value()));
            }
            
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


namespace {
    HPS::RGBAColor getColor( A3DUns32 const &color_idx, double const &a ) {
        if( A3D_DEFAULT_COLOR_INDEX == color_idx ) {
            return HPS::RGBAColor( 1., 0., 0., 1. );
        }
        A3DGraphRgbColorData rgb_color_data;
        A3D_INITIALIZE_DATA( A3DGraphRgbColorData, rgb_color_data );
        A3DGlobalGetGraphRgbColorData( color_idx, &rgb_color_data );
        auto const &r = rgb_color_data.m_dRed;
        auto const &g = rgb_color_data.m_dGreen;
        auto const &b = rgb_color_data.m_dBlue;
        return HPS::RGBAColor( r, g, b, a );
    }
}

HPS::MaterialKit SceneGraphBuilder::createMaterialKit( A3DGraphStyleData const &style_data ) {
    HPS::MaterialKit mk;
    if( style_data.m_bMaterial ) {
        A3DBool is_texuture = false;
        A3DGlobalIsMaterialTexture( style_data.m_uiRgbColorIndex, &is_texuture );
        if( is_texuture ) {
            A3DGraphTextureDefinitionData texture_def;
            A3D_INITIALIZE_DATA( A3DGraphTextureDefinitionData, texture_def );
            A3DGlobalGetGraphTextureDefinitionData( style_data.m_uiRgbColorIndex, &texture_def );
            
            A3DGraphPictureData picture;
            A3D_INITIALIZE_DATA( A3DGraphPictureData, picture );
            A3DGlobalGetGraphPictureData( texture_def.m_uiPictureIndex, &picture );
            
            // TODO push the texture data into the portfolio
            // apply style to segment
            
        } else {
            A3DGraphMaterialData material_data;
            A3D_INITIALIZE_DATA( A3DGraphMaterialData, material_data );
            A3DGlobalGetGraphMaterialData( style_data.m_uiRgbColorIndex, &material_data );
            auto const ambient_color = getColor( material_data.m_uiAmbient, material_data.m_dAmbientAlpha );
            auto const diffuse_color = getColor( material_data.m_uiDiffuse, material_data.m_dDiffuseAlpha );
            if( ambient_color.alpha == 1. && diffuse_color.alpha == 0. ) {
                mk.SetDiffuse( ambient_color );
            } else if( ambient_color.alpha == 0. && diffuse_color.alpha == 1. ) {
                mk.SetDiffuse( diffuse_color );
            } else {
                mk.SetDiffuse( HPS::Interpolate( ambient_color, diffuse_color, 0.5 ) );
            }
            mk.SetEmission( getColor( material_data.m_uiEmissive, material_data.m_dEmissiveAlpha ) );
            mk.SetSpecular( getColor( material_data.m_uiSpecular, material_data.m_dSpecularAlpha ) );
        }
    } else {
        auto const a = static_cast<double>(style_data.m_bIsTransparencyDefined ? style_data.m_ucTransparency : 255u) / 255.;
        auto const color = getColor( style_data.m_uiRgbColorIndex, a );
        mk.SetDiffuse( color );
        mk.SetSpecular( HPS::RGBAColor::Nothing() );
        mk.SetEmission( HPS::RGBAColor::Nothing() );
    }
    return mk;
}
