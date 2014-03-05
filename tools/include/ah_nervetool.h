#ifndef	INCLUDE_AH_NERVETOOL_H
#define INCLUDE_AH_NERVETOOL_H

/*#########################################################################*/
/*#########################################################################*/

#include <ah_tools.h>

class NerveTool;
class Nerve;

/*#########################################################################*/
/*#########################################################################*/

class AdminApi;

class NerveTool : public ToolBase {
public:
	virtual int execute( int nargs , char **args );

public:
	NerveTool();
	virtual ~NerveTool();

// internals
private:
	void execute_file( FILE *sin , FILE *sout );
	void execute_data( StringList& data , FILE *sout );
	int execute_line( String& s , FILE *sout , ClassList<Nerve>& nn );
	int execute_img( String& s , FILE *sout , int level , ClassList<Nerve>& nn );

	bool extract_item( FILE *sout , String& item , String& s , String p );
	bool extract_fibers( FILE *sout , StringList& fibersinfo , String& s );
	bool extract_fiberitems( FILE *sout , StringList& fibersinfo , String type , String& value );
	bool extract_codes( String& s , StringList *z );

	void addManyToOne( StringList& fibersinfo , String type , StringList& from , String to );
	void addOneToMany( StringList& fibersinfo , String type , String from , StringList& to );
	void addSingleChain( StringList& fibersinfo , String type , ClassList<StringList>& chain , int from , int to );
	void addFiberItem( StringList& fibersinfo , String type , String from , String to , StringList& mids );

	void output_data( FILE *sout , ClassList<Nerve>& nn );
	void output_nerve( FILE *sout , ClassList<Nerve>& nn , int pos );

private:
	String name;
};

/*#########################################################################*/
/*#########################################################################*/

class Nerve {
public:
	Nerve() { level = 0; };

public:
	int level;
	String name;
	String synonyms;
	String origin;
	String branches;
	String distribution;
	String imginfo;
	String modality;
	String fibers;
	StringList fibersinfo;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_NERVETOOL_H
