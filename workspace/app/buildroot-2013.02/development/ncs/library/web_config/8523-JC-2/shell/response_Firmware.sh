FILE_PATH=/var/www/uploads
FILE_NAME=$(ls -1rt $FILE_PATH | head -1)
cd $FILE_PATH
mv $FILE_NAME /tmp/       
cd /tmp
upgrade -d $FILE_NAME                           
chmod +x upgrade.sh && ./upgrade.sh  
/sbin/reboot
