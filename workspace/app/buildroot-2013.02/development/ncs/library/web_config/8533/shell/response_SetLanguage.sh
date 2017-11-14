web_tool 0
cp ../lan/$1/*.htm ../cache/
cp ../ini/$1.lan ../ini/string.ini
../cgi-bin/iniUpdate 0 ../x2i/set_language.x2i 1
#../cgi-bin/iniUpdate 0 ../x2i/iniUpdate.x2i 0
