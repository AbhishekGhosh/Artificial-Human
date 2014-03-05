#ifndef	INCLUDE_AH_MINDBASE_H
#define INCLUDE_AH_MINDBASE_H

/*#########################################################################*/
/*#########################################################################*/

#include <platform/include/ah_platform.h>

/*#########################################################################*/
/*#########################################################################*/

// signal intensity
#define NEUROVT_NORMAL_SIGNAL_STRENGTH 10

// membrane, action potential, absolute charge value in nQ
// normal threshold to fire=200
// state can be negative
// also for intermediate calculation
typedef signed short neurovt_state;

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_MINDBASE_H
