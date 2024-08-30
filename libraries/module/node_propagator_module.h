#ifndef NODE_PROPAGATOR_MODULE_H
#define NODE_PROPAGATOR_MODULE_H

// This module provides propagtion of states for a node

#include "physics/simulatorclass.h"

namespace Cosmos
{
    namespace Module
    {
        class NodePropagatorModule
        {
        public:

            NodePropagatorModule() {}

            int32_t Init(Support::cosmosstruc *cinfo);

            
        private:
            Physics::Simulator *sim = nullptr;
        };
    }
    
}

#endif // NODE_PROPAGATOR_MODULE_H
