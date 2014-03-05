#include <ah_platform.h>
#include <windows.h>

// #############################################################################
// #############################################################################

// SEH exception handling
RuntimeException::RuntimeException( unsigned int code , int skipLevels , void *p_addr ) {
	seh = true;

	// catch stack
	stack = rfc_thr_stackget( skipLevels + 1 );

	file = "";
	fileShort = "";
	line = 0;

	// get exception address
	char l_className[ 300 ];
	char l_functionName[ 300 ];
	if( !rfc_thr_stacknamebyaddr( p_addr , l_className , l_functionName ) )
		*l_className = *l_functionName = 0;
	className = l_className;
	functionName = l_functionName;

	// translate SEH exception code
	switch( code ) {
		case EXCEPTION_ACCESS_VIOLATION : msg = "EXCEPTION_ACCESS_VIOLATION"; break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED : msg = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED"; break;
		case EXCEPTION_BREAKPOINT : msg = "EXCEPTION_BREAKPOINT"; break;
		case EXCEPTION_DATATYPE_MISALIGNMENT : msg = "EXCEPTION_DATATYPE_MISALIGNMENT"; break;
		case EXCEPTION_FLT_DENORMAL_OPERAND : msg = "EXCEPTION_FLT_DENORMAL_OPERAND"; break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO : msg = "EXCEPTION_FLT_DIVIDE_BY_ZERO"; break;
		case EXCEPTION_FLT_INEXACT_RESULT : msg = "EXCEPTION_FLT_INEXACT_RESULT"; break;
		case EXCEPTION_FLT_INVALID_OPERATION : msg = "EXCEPTION_FLT_INVALID_OPERATION"; break;
		case EXCEPTION_FLT_OVERFLOW : msg = "EXCEPTION_FLT_OVERFLOW"; break;
		case EXCEPTION_FLT_STACK_CHECK : msg = "EXCEPTION_FLT_STACK_CHECK"; break;
		case EXCEPTION_FLT_UNDERFLOW : msg = "EXCEPTION_FLT_UNDERFLOW"; break;
		case EXCEPTION_ILLEGAL_INSTRUCTION : msg = "EXCEPTION_ILLEGAL_INSTRUCTION"; break;
		case EXCEPTION_IN_PAGE_ERROR : msg = "EXCEPTION_IN_PAGE_ERROR"; break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO : msg = "EXCEPTION_INT_DIVIDE_BY_ZERO"; break;
		case EXCEPTION_INT_OVERFLOW : msg = "EXCEPTION_INT_OVERFLOW"; break;
		case EXCEPTION_INVALID_DISPOSITION : msg = "EXCEPTION_INVALID_DISPOSITION"; break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION : msg = "EXCEPTION_NONCONTINUABLE_EXCEPTION"; break;
		case EXCEPTION_PRIV_INSTRUCTION : msg = "EXCEPTION_PRIV_INSTRUCTION"; break;
		case EXCEPTION_SINGLE_STEP : msg = "EXCEPTION_SINGLE_STEP"; break;
		case EXCEPTION_STACK_OVERFLOW : msg = "EXCEPTION_STACK_OVERFLOW"; break;
		default:
			msg = "UNKNOWN CODE";
			break;
	}
}

// C++ exception handling
RuntimeException::RuntimeException( const char *p_msg , const char *p_file , int p_line ) {
	seh = false;

	msg = p_msg;
	file = p_file;
	stack = rfc_thr_stackget( 3 );
	line = p_line;
	
	const char *p1 = strrchr( file , '\\' );
	const char *p2 = strrchr( file , '/' );

	if( p1 == NULL && p2 == NULL )
		fileShort = file;
	else
	if( p1 != NULL && p2 == NULL )
		fileShort = p1 + 1;
	else
	if( p1 == NULL && p2 != NULL )
		fileShort = p2 + 1;
	else
	if( p1 < p2 )
		fileShort = p2 + 1;
	else
		fileShort = p1 + 1;
}

String RuntimeException::printStackToString() {
	String error;
	if( !isSEH() )
		error = String( "C++ Exception: " ) + msg + " (file=" + fileShort + ", line=" + line + ")";
	else
		error = String( "SEH Exception: " ) + className + "::" + functionName + " (" + msg + ")";

	// skip system levels
	int skipCount = getSkipCount();
	int startItem = rfc_thr_stackfulldepth( stack ) - 1;
	if( skipCount > 0 ) {
		error += String( "\n\t...skipped..." );
		startItem -= skipCount;
	}

	for( int k = startItem; k >= 0; k-- ) {
		rfc_threadstacklevel *sl = rfc_thr_stacklevel( stack , k );

		// extract short name
		String moduleName = sl -> moduleName;
		int from = moduleName.findLastAny( "/\\" );
		int to = moduleName.findLast( '.' );

		String moduleNameShort = moduleName;
		if( from >= 0 && to >= 0 )
			moduleNameShort = moduleNameShort.getMid( from + 1 , to - from - 1 );

		error += String( "\n\t" ) + sl -> className + 
			"::" + sl -> functionName + 
			" (" + moduleNameShort + 
			", " + sl -> message + ")";

		// stop after main function or after start thread function
		if( strcmp( sl -> functionName , "_main" ) == 0 ||
			strcmp( sl -> functionName , "threadMainFunction" ) == 0 ||
			strcmp( sl -> functionName , "runThread" ) == 0 ) {
			error += String( "\n\t...skipped..." );
			break;
		}
	}

	return( error );
}

int RuntimeException::getSkipCount() {
	// show all stack if there was error in getting stack
	if( stack -> brokenStack )
		return( 0 );

	// skip stack getting functions
	int skipCount = rfc_thr_stackfulldepth( stack ) - rfc_thr_stackdepth( stack );

	// skip system functions
	for( int k = rfc_thr_stackdepth( stack ) - 1; k >= 0; k-- , skipCount++ ) {
		rfc_threadstacklevel *sl = rfc_thr_stacklevel( stack , k );
		if( sl -> moduleName == NULL )
			continue;

		if( sl -> className != NULL )
			break;

		if( sl -> functionName == NULL )
			continue;

		if( !strcmp( sl -> functionName , "_CallSETranslator" ) )
			continue;
					
		if( !strcmp( sl -> functionName , "___InternalCxxFrameHandler" ) )
			continue;

		if( !strcmp( sl -> functionName , "___CxxFrameHandler" ) )
			continue;

		break;
	}

	return( skipCount );
}

bool RuntimeException::isSEH() {
	return( seh );
}

const char *RuntimeException::getClassName() {
	return( className );
}

const char *RuntimeException::getFunctionName() {
	return( functionName );
}

const char *RuntimeException::getMsg() {
	return( msg );
}

const char *RuntimeException::getFileShort() {
	return( fileShort );
}

int RuntimeException::getLine() {
	return( line );
}

rfc_threadstack *RuntimeException::getStack() {
	return( stack );
}


