#include <ah_platform.h>

/*#########################################################################
 * This class is takes care of the session. The sessions are stored in the 
 * hashmap kind of datastructure.
/*#########################################################################*/

/*#########################################################################*/
/*#########################################################################*/

/*
 * This is the constructor that will initialise the session object with
 * the passed ID.
 * @param Session ID
 */
MessageSession::MessageSession( int id ) {
	sessionId = id;
	dataLock = rfc_lock_create();
}

/*
 * Destructor that will destroy any mapping between objects and
 * session names.
 */
MessageSession::~MessageSession() {
	rfc_lock_destroy( dataLock );
	objects.destroy();
}

/*
 * Returns the session ID
 */
int MessageSession::getSessionId() {
	return( sessionId );
}

/*
 * Adds the mapping between the object and the name.
 * @param o is the session object
 * @param name is the session name
 */
void MessageSession::setObject( Object *o , String name ) {
	rfc_lock_exclusive( dataLock );
	Object *oOld = objects.get( name );	
	objects.add( name , o );
	rfc_lock_release( dataLock );

	ASSERT( oOld == NULL );
}

/*
 * Returns the session given the name.
 * @param name is the session name.
 */
Object *MessageSession::getObject( String name ) {
	rfc_lock_shared( dataLock );
	Object *o = objects.get( name );
	rfc_lock_release( dataLock );

	return( o );
}
