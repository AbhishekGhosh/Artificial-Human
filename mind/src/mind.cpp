#include <ah_mind.h>

/*#########################################################################*/
/*#########################################################################*/

// any object or definition class has constructor with parameter - owner class
//		constructor should do nothing except setting initial member values
//		no contained objects are created
// any object class has configure() method with parameter - prototype definition class
//		configure should define mode of creation
// any object class has create() method with additional parameters if required
//		create method should create contained all objects
//		no references are created, except parameters
// any object class can have additional link methods to create specific references
