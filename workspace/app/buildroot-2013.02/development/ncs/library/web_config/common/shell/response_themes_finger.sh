cp ../themes/finger/* ../ -f -r
grep "language=" ../ini/sys_cfg.ini | ../shell/response_SetLanguage.sh `sed 's/language=//'`
