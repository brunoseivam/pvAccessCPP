/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * pvAccessCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */

#ifndef CAPROVIDER_H
#define CAPROVIDER_H

#include <shareLib.h>
#include <pv/pvAccess.h>

namespace epics {
namespace pvAccess {
namespace ca {

/**
 * @brief  CAClientFactory is a channel provider for the ca network provider.
 *
 * 
 */
class epicsShareClass CAClientFactory
{
public:
    /** @brief start the provider
     *
     */
    static void start();
    /** @brief stop the provider
     *
     */
    static void stop();
};

}
}
}

#endif  /* CAPROVIDER_H */
