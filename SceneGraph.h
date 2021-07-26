#pragma once

#include <hps.h>
#include <A3DSDKIncludes.h>
#include "SceneGraph.h"

struct SceneGraph {
    SceneGraph() {};
    ~SceneGraph() {};
    
    HPS::SegmentKey root_key;
};
