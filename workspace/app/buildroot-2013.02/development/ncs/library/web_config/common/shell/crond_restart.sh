mkdir /var/spool/cron
mkdir /var/spool/cron/crontabs
crontab /var/www/shell/watchdog_for_linphone.txt
crontab -l
crond restart
