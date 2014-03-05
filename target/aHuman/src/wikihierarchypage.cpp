#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

WikiHierarchyPage::WikiHierarchyPage( WikiMaker *p_wm ) {
	attachLogger();
	wm = p_wm;
}

WikiHierarchyPage::~WikiHierarchyPage() {
}

void WikiHierarchyPage::execute() {
	bool createPageHierarchy = wm -> wiki.getBooleanProperty( "createPageHierarchy" , true );
	if( createPageHierarchy == false ) {
		logger.logInfo( "skip creating hierarchy page" );
		return;
	}

	// get wiki file
	String wikiDir = wm -> wiki.getProperty( "wikiPath" );
	String wikiPage = wm -> wiki.getProperty( "wikiPageHierarchy" );

	// create hierarchy section
	StringList divisions;
	wm -> hmindxml.getDivisions( divisions );
	for( int k = 0; k < divisions.count(); k++ ) {
		String division = divisions.get( k );
		if( division.equals( "Cortex" ) )
			updateHierarchyPage_walkNeocortex( division , wikiDir , wikiPage );
		else {
			StringList lines;
			updateHierarchyPage_walkTree( division , 0 , lines , NULL , NULL );
			wm -> updateFileSection( wikiDir , wikiPage , division , lines );
		}
	}
}

void WikiHierarchyPage::updateHierarchyPage_walkTree( String parentNode , int level , StringList& lines , MindArea *parentArea , MindRegion *parentRegion ) {
	StringList elements;
	wm -> hmindxml.getIdentifiedElements( parentNode , elements );

	for( int k = 0; k < elements.count(); k++ ) {
		String node = elements.get( k );

		// handle mapping
		MindArea *ownArea = NULL;
		MindRegion *ownRegion = NULL;

		if( parentArea == NULL )
			ownArea = updateHierarchyPage_getArea( node );
		if( parentRegion == NULL )
			ownRegion = updateHierarchyPage_getRegion( node );

		// add item string
		String itemString = updateHierarchyPage_getElementString( node , parentArea , parentRegion , ownArea , ownRegion );
		lines.add( String( " " ).replicate( 2 + level ) + "* " + itemString );

		// walk down
		if( ownArea == NULL )
			ownArea = parentArea;
		if( ownRegion == NULL )
			ownRegion = parentRegion;

		updateHierarchyPage_walkTree( node , level + 1 , lines , ownArea , ownRegion );
	}
}

MindArea *WikiHierarchyPage::updateHierarchyPage_getArea( String parentNode ) {
	MindService *ms = MindService::getService();

	const XmlHMindElementInfo& info = wm -> hmindxml.getElementInfo( parentNode );

	// check ignored
	if( info.ignore == true )
		return( NULL );

	// check direct
	if( info.mapped ) {
		MindRegion *region = ms -> getMindRegion( info.id );
		return( region -> getArea() );
	}

	// check all childs
	MindArea *area = NULL;
	StringList elements;
	wm -> hmindxml.getIdentifiedElements( parentNode , elements );

	for( int k = 0; k < elements.count(); k++ ) {
		String node = elements.get( k );

		// check ignored
		const XmlHMindElementInfo& info = wm -> hmindxml.getElementInfo( node );
		if( info.ignore == true )
			continue;

		MindArea *one = updateHierarchyPage_getArea( node );
		if( area == NULL )
			area = one;
		else if( one != area )
			return( NULL );
	}

	return( area );
}

MindRegion *WikiHierarchyPage::updateHierarchyPage_getRegion( String parentNode ) {
	MindRegion *region = NULL;
	MindService *ms = MindService::getService();

	const XmlHMindElementInfo& info = wm -> hmindxml.getElementInfo( parentNode );

	// check ignored
	if( info.ignore == true )
		return( NULL );

	// check direct
	if( info.mapped ) {
		region = ms -> getMindRegion( info.id );
		return( region );
	}

	// check all childs
	StringList elements;
	wm -> hmindxml.getIdentifiedElements( parentNode , elements );

	for( int k = 0; k < elements.count(); k++ ) {
		String node = elements.get( k );

		// check ignored
		const XmlHMindElementInfo& info = wm -> hmindxml.getElementInfo( node );
		if( info.ignore == true )
			continue;

		MindRegion *one = updateHierarchyPage_getRegion( node );
		if( region == NULL )
			region = one;
		else if( one != region )
			return( NULL );
	}

	return( region );
}

String WikiHierarchyPage::updateHierarchyPage_getElementString( String node , MindArea *parentArea , MindRegion *parentRegion , MindArea *ownArea , MindRegion *ownRegion ) {
	String value;

	const XmlHMindElementInfo& info = wm -> hmindxml.getElementInfo( node );

	// own name if any
	if( !info.name.isEmpty() ) {
		if( info.mapped )
			value = "[" + wm -> getRegionPage( info.id ) + " " + info.name + "] *";
		else
			value = "*" + info.name;

		if( !info.id.isEmpty() ) {
			String regionRole;
			if( ownRegion != NULL )
				regionRole = ", " + ownRegion -> getRegionInfo() -> getRoleName();

			value += " (" + info.id + regionRole + ")";
		}

		value += "*";
	}

	// ref section
	if( !info.refs.isEmpty() ) {
		StringList refList;
		info.refs.split( refList , ";" );
		for( int k = 0; k < refList.count(); k++ ) {
			String ref = refList.get( k );
			ref.trim();

			if( !value.isEmpty() )
				value += " = ";

			value += ref;
		}
	}

	// mapping section
	if( ownArea != NULL )
		value += " -> [BrainArea" + ownArea -> getAreaId() + "]";

	// ignore section
	if( info.ignore == true ) {
		value += " (ignore";
		if( !info.comment.isEmpty() )
			value += ", " + info.comment;
		value += ")";
	}
	else if( !info.comment.isEmpty() )
		value += " (" + info.comment + ")";

	return( value );
}

void WikiHierarchyPage::updateHierarchyPage_walkNeocortex( String neocortexDivision , String wikiDir , String wikiPage ) {
	StringList elements;
	wm -> hmindxml.getIdentifiedElements( neocortexDivision , elements );

	// group by major lobes
	for( int k = 0; k < elements.count(); k++ ) {
		String node = elements.get( k );
		const XmlHMindElementInfo& info = wm -> hmindxml.getElementInfo( node );

		StringList lines;
		updateHierarchyPage_getNeocortexLobeLines( node , lines );
		wm -> updateFileSection( wikiDir , wikiPage , info.name , lines );
	}
}

void WikiHierarchyPage::updateHierarchyPage_getNeocortexLobeLines( String neocortexLobe , StringList& lines ) {
	String banum;
	banum.resize( 10 );

	// list by brodmann areas
	MapStringToString balines;
	for( int k = 1; k < 100; k++ ) {
		sprintf( banum.getBuffer() , "%2.2d" , k );

		String baline = updateHierarchyPage_getNeocortexBrodmannLine( neocortexLobe , banum );
		if( !baline.isEmpty() )
			balines.add( banum , baline );
	}

	// group the same if sequential
	String lastnumset;
	String lastline;
	for( int k = 1; k < 100; k++ ) {
		sprintf( banum.getBuffer() , "%2.2d" , k );
		String baline = balines.get( banum );

		if( baline.isEmpty() && k < 99 )
			continue;

		// first
		if( lastnumset.isEmpty() ) {
			if( baline.isEmpty() )
				continue;

			lastnumset = String( "BA " ) + k;
			lastline = baline;
			continue;
		}

		// next - check the same
		if( lastline.equals( baline ) ) {
			lastnumset += String( "," ) + k;
			continue;
		}

		// add line
		String value = "  * *" + lastnumset + "*: " + lastline;
		lines.add( value );

		// set next
		if( baline.isEmpty() ) {
			lastnumset.clear();
			lastline.clear();
		}
		else {
			lastnumset = String( "BA " ) + k;
			lastline = baline;
		}
	}
}

String WikiHierarchyPage::updateHierarchyPage_getNeocortexBrodmannLine( String neocortexDivision , String banum ) {
	// check all childs
	StringList items;
	updateHierarchyPage_walkNeocortexBrodmannLine( neocortexDivision , banum , items );

	// no items
	if( items.count() == 0 )
		return( "" );

	// make string
	String value;
	for( int k = 0; k < items.count(); k++ ) {
		if( !value.isEmpty() )
			value += ", ";
		value += items.get( k );
	}

	return( value );
}

void WikiHierarchyPage::updateHierarchyPage_walkNeocortexBrodmannLine( String node , String banum , StringList& items ) {
	const XmlHMindElementInfo& info = wm -> hmindxml.getElementInfo( node );

	if( info.brodmannid.isEmpty() ) {
		// check childs
		StringList elements;
		wm -> hmindxml.getIdentifiedElements( node , elements );

		// group by major lobes
		for( int k = 0; k < elements.count(); k++ ) {
			String nodeChild = elements.get( k );
			updateHierarchyPage_walkNeocortexBrodmannLine( nodeChild , banum , items );
		}
		return;
	}

	// check itself
	if( info.brodmannid.find( banum ) < 0 )
		return;

	// add string
	String value;
	if( info.ignore ) {
		value = "ignored";
		if( !info.comment.isEmpty() )
			value += ", " + info.comment;
	}
	else {
		MindService *ms = MindService::getService();
		MindRegion *region = ms -> getMindRegion( info.id );
		MindArea *area = region -> getArea();

		value = "[" + wm -> getRegionPage( info.id ) + " " + info.name + " (" + info.id + ")]";
	}

	items.add( value );
}
