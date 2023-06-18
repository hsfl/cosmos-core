#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "support/configCosmos.h"
#include <cmath>
namespace Cosmos {
    namespace Physics {


        class Constants
        {
        public:
            Constants();
            static const double G;
            static const double massEarth;
            static const double radiusEarth; //km
            static const double mu; //
        };
    }
}
#endif // CONSTANTS_H
