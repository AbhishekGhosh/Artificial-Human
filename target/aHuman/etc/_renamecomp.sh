#!/bin/sh

P_SRC=$1
P_DST=$2

if [ "$P_SRC" = "" ] || [ "$P_DST" = "" ]; then
	echo invalid params. Exiting
	exit 1
fi

function f_exec() {
	P_FNAME=$1

	if [ "`grep \"$P_SRC\" $P_FNAME`" != "" ]; then
		echo execute for file: $P_FNAME ...
		sed "s/\"$P_SRC\"/\"$P_DST\"/g;s/=$P_SRC /=$P_DST /g;s/=$P_SRC\"/=$P_DST\"/g" $P_FNAME > $P_FNAME.in
		rm $P_FNAME
		mv $P_FNAME.in $P_FNAME
	fi
}

P_SRCWIKI=`echo $P_SRC | sed "s/[.]/_/g"`
P_DSTWIKI=`echo $P_DST | sed "s/[.]/_/g"`

for x in `find circuits -name "*\.xml"`; do
	f_exec $x
done

for x in `find nerves -name "*\.xml"`; do
	f_exec $x
done

F_SPECFILES="hmind.xml mind.xml mindnet.xml target.xml"
for x in $F_SPECFILES; do
	f_exec $x
done

# rename pages
cd ../../../..

if [ -f wiki/BrainArea${P_SRCWIKI}.wiki ]; then
	svn rename wiki/BrainArea${P_SRCWIKI}.wiki wiki/BrainArea${P_DSTWIKI}.wiki

	for x in `find wiki -name "BrainRegion${P_SRCWIKI}*" -maxdepth 1`; do
		xnew=`echo $x | sed "s/BrainRegion${P_SRCWIKI}/BrainRegion${P_DSTWIKI}/"`
		svn rename $x $xnew
	done

	for x in `find images/dot/aHuman -name "${P_SRC}\.*" -maxdepth 1`; do
		xnew=`echo $x | sed "s/^${P_SRC}./${P_DST}./"`
		svn rename $x $xnew
	done
fi

if [ -f wiki/BrainRegion${P_SRCWIKI}.wiki ]; then
	svn rename wiki/BrainRegion${P_SRCWIKI}.wiki wiki/BrainRegion${P_DSTWIKI}.wiki
	svn rename images/dot/aHuman/${P_SRC}.dot.jpg images/dot/aHuman/${P_DST}.dot.jpg
fi
