#ifndef stepper_sim_Platform_hpp
#define stepper_sim_Platform_hpp

/**\file
 * This file provides constants and typedefs to configure the common
 * code for a specific platform.
 */

#include <boost/thread.hpp>
#include <Dictionary.hpp>
#include <MemoryAllocator.hpp>
#include <Communicator.hpp>

namespace stepper { namespace device { namespace platform {

typedef boost::mutex Lock;

enum {
	/**
	 * The number of cached script messages for the engine (minimum 4)
	 * Uses about 270 bytes each.
	 */
	SCRIPT_MSG_POOL = 4
};

typedef MemoryAllocator<Lock, 2> MemoryAllocator_type;
MemoryAllocator_type &getMemoryAllocator();

typedef Communicator<Lock> Communicator_type;
Communicator_type &getCommunicator();

void resetPlatform();


}}}

#endif
