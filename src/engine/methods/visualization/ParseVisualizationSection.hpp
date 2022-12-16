#ifndef parsevisualizationsection_HPP
#define parsevisualizationsection_HPP

#ifndef ANT_CONFIG_H
#include "config.h"
#endif

#if ANT_HAS_VIS

#include "../../../antvis/buffer.h"
#include "../../../antvis/visualization.h"
#include "../../../utils/config/Configuration.hpp"

TArrayBuffer* parseVisualizationSection (TVisualization* vis, 
					 Configuration& ini, 
					 int bufferColumns);
#endif

#endif
