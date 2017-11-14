rm -fr /var/lock
rm -fr /var/run
#rm -fr /var/log
mkdir /var/lock
mkdir /var/run
#mkdir /var/log
mkdir /var/log/boa
touch /var/log/boa/access_log
touch /var/log/boa/error_log
chmod 777 /var/www/*
chmod 777 /var/www/cgi-bin/*
chmod 777 /var/www/shell/*
chmod 777 /var/www/ini/*
chmod 777 /var/www/x2i/*
chmod 777 /var/www/c2h/*
chmod 777 /var/www/log/*
chmod 777 /var/www/cache/*
#grep "language=" /var/www/ini/sys_cfg.txt | /var/www/shell/response_SetLanguage.sh `sed 's/language=//'`
/usr/sbin/boa
