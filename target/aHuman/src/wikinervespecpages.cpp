#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

WikiNerveSpecPages::WikiNerveSpecPages( WikiMaker *p_wm ) {
	attachLogger();
	wm = p_wm;
}

WikiNerveSpecPages::~WikiNerveSpecPages() {
}

void WikiNerveSpecPages::execute() {
	bool createMainPage = wm -> wiki.getBooleanProperty( "createNervePages" , true );
	if( createMainPage == false ) {
		logger.logInfo( "skip creating nerve spec pages" );
		return;
	}

	// get nerves from pns
	ClassList<XmlNerveInfo>& divs = wm -> nervesxml.getDivisions();

	// collect model circuit section lines
	for( int k = 0; k < divs.count(); k++ )
		addNerveDivision( divs.getRef( k ) );
}

void WikiNerveSpecPages::addNerveDivision( XmlNerveInfo& div ) {
	StringList lines;
	String divPage = div.origin;
	String divSection = div.name;

	for( int k = 0; k < div.childs.count(); k++ )
		addNerveList( 0 , div.childs.getClassRefByIndex( k ) , lines );

	String wikiDir = wm -> wiki.getProperty( "wikiPath" );
	wm -> updateFileSection( wikiDir , divPage , divSection , lines );
}

void WikiNerveSpecPages::addNerveList( int level , XmlNerveInfo& nerve , StringList& lines ) {
	// item string
	String s = String( " " ).replicate( level + 2 ) + "* *" + nerve.name + "*";
	if( !nerve.synonyms.isEmpty() )
		s += " (" + nerve.synonyms + ")";
	if( !nerve.origin.isEmpty() )
		s += "; ORIGIN={" + nerve.origin + "}";
	if( !nerve.branches.isEmpty() )
		s += "; BRANCHES={" + nerve.branches + "}";
	if( !nerve.distribution.isEmpty() )
		s += "; DISTRIBUTION={" + nerve.distribution + "}";
	if( !nerve.modality.isEmpty() )
		s += "; MODALITY={" + nerve.modality + "}";
	
	String muscles = getNerveDivision_muscles( nerve.name );
	if( !muscles.isEmpty() )
		s += "; MUSCLES={" + muscles + "}";

	if( nerve.fibers.count() > 0 )
		s += "; FIBERS={" + getNerveDivision_fibers( nerve.fibers ) + "}";
	if( nerve.rootNerves.count() > 0 )
		s += "; NERVES={" + getNerveDivision_rootNerves( nerve.rootNerves ) + "}";

	lines.add( s );

	// image
	if( !nerve.imgsrc.isEmpty() ) {
		s = String( " " ).replicate( level + 3 ) + "* " + wm -> getImageWikiLink( nerve.imgsrc , nerve.imgheight );
		lines.add( s );
	}

	// childs
	for( int k = 0; k < nerve.childs.count(); k++ )
		addNerveList( level + 1 , nerve.childs.getClassRefByIndex( k ) , lines );
}

String WikiNerveSpecPages::getNerveDivision_fibers( ClassList<XmlNerveFiberInfo>& fibers ) {
	String s;
	for( int k = 0; k < fibers.count(); k++ ) {
		XmlNerveFiberInfo& fiber = fibers.getRef( k );
		if( k > 0 )
			s += "; ";
		s += fiber.type + ":" + getNerveDivision_fiberchain( fiber );
	}
	return( s );
}

String WikiNerveSpecPages::getNerveDivision_rootNerves( StringList rootNerves ) {
	String s;
	for( int k = 0; k < rootNerves.count(); k++ ) {
		String nerve = rootNerves.get( k );
		if( k > 0 )
			s += "; ";

		XmlNerveInfo& nf = wm -> nervesxml.getNerveInfo(  nerve );
		s += "[" + nf.div -> origin + " " + nf.name + "]";
	}
	return( s );
}

String WikiNerveSpecPages::getNerveDivision_fiberchain( XmlNerveFiberInfo& fiber ) {
	String s;

	String comp;
	for( int k = 0; k < fiber.mids.count() + 2; k++ ) {
		if( k == 0 )
			comp = fiber.src;
		else {
			s += " -> ";

			if( k > fiber.mids.count() )
				comp = fiber.dst;
			else
				comp = fiber.mids.get( k - 1 );
		}

		String region = wm -> hmindxml.getMappedRegion( comp );
		
		const XmlHMindElementInfo& ni = wm -> hmindxml.getElementInfo( comp );
		s += "[BrainRegion" + region.replace( "." , "_" ) + " " + ni.name + "," + comp + "]";
	}

	return( s );
}

String WikiNerveSpecPages::getNerveDivision_muscles( String name ) {
	StringList muscles;
	wm -> musclesxml.getMusclesByNerve( name , muscles );

	if( muscles.count() == 0 )
		return( "" );

	// form muscle string
	muscles.sort();
	String ms;
	for( int k = 0; k < muscles.count(); k++ ) {
		String muscle = muscles.get( k );
		XmlMuscleInfo& info = wm -> musclesxml.getMuscleInfo( muscle );

		if( k > 0 )
			ms += "; ";

		ms += "[" + info.link + " " + info.name + "] - " + info.action;
	}

	return( ms );
}

