#include <ah_platform.h>
#include <ah_testing_impl.h>

/*#########################################################################*/
/*#########################################################################*/

TestUnit::TestUnit( String p_name ) {
	testUnitName = p_name;
	initialized = false;
}

TestUnit::~TestUnit() {
	methods.destroy();
}

TestMethod *TestUnit::getMethod( String methodName ) {
	return( methods.get( methodName ) );
}
	
void TestUnit::callMethod( TestMethod *method , XmlCall& call ) {
	// if first execution - create unit
	if( !initialized ) {
		initialized = true;
		onInit();
	}

	// execute method
	( *this.*(method->fptr) )( call );
}
	
void TestUnit::addMethod( String methodName , TestUnitMethodType methodFunction ) {
	methods.add( methodName , new TestMethod( methodName , methodFunction ) );
}
