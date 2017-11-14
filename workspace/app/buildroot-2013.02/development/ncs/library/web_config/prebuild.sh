if [ -e $2 ]
then
echo "*******************cd $2"
cd $2


echo "*************Generate version file"
VER_FILE=$2/common/ini/WebInfo.txt

xml=`svn info --xml --incremental`
revision=`echo "$xml"|sed '/revision/!d'|sed '$d'`
BUILD_VER=`echo $revision|sed 's/revision="\([0-9]\+\)">\?/\1/'`

YEAR=$(date +%F | cut -d'-' -f1)
MON=$(date +%F | cut -d'-' -f2)
DAY=$(date +%F | cut -d'-' -f3)
#read -p "Please enter version(e.g. x.x.x.x):" RELEASE_VER
#MAJOR=$(echo $RELEASE_VER | cut -d'.' -f1)
#MINOR=$(echo $RELEASE_VER | cut -d'.' -f2)
#INNER=$(echo $RELEASE_VER | cut -d'.' -f3)
#REVISE=$(echo $RELEASE_VER | cut -d'.' -f4)
MAJOR=1
MINOR=0
INNER=0
REVISE=0

if [ -z $MAJOR ] || [ -z $MINOR ] || [ -z $INNER ] || [ -z $REVISE ]
then
	echo "Release version formate error"
	exit 1
fi
#read -p "Please enter version info[标准]:" VERINFO
if [ "$VERINFO" ]
then
	:
else
	VERINFO=标准
fi

echo "[ncs]" > $VER_FILE
echo -e "chrome=1\n" >> $VER_FILE

echo "[version_info]" >> $VER_FILE
echo "devName=WEB" >> $VER_FILE
echo "verMajor=$MAJOR" >> $VER_FILE
echo "verMinor=$MINOR" >> $VER_FILE
echo "verInner=$INNER" >> $VER_FILE
echo "verRev=$REVISE" >> $VER_FILE
echo "verBuild=$BUILD_VER" >> $VER_FILE
echo "verInfo=$VERINFO" >> $VER_FILE
echo "verYear=$YEAR" >> $VER_FILE
echo "verMonth=$MON" >> $VER_FILE
echo "verDay=$DAY" >> $VER_FILE
cat $VER_FILE


if [ -e $1 ]
then
	echo "**********************************"
	echo "********************************$1"
	echo "******************remove www files"
	rm www/* -fr
	echo "*******************cp common files"
	cp common/* www/ -fr
	echo "***********************cp $1 files"
	cp $1/* www/ -fr
	echo "***********run tool_SetLanguage.sh"
	cd www/shell
	./tool_SetLanguage.sh 1
	echo "*************************successful"
	echo "***********************************"
else
	echo "**********************************"
	echo "format input error"
	echo "prebuild.sh num"
	echo "num like 3081 8523 8522 and so on"
	echo "by nicholas"
	echo "***********************************"
fi
else
	echo "************************$2 not found"
fi
