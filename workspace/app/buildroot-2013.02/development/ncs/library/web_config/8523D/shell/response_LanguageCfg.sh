
grep "language=" ../ini/sys_cfg.txt | ../shell/response_SetLanguage.sh `sed 's/language=//'`
cp ../ini/sys_cfg.txt /mnt/nand1-2/www/ini/ -f
web_tool 0
