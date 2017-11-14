UPG_FILE=spon.upg
FILE_PATH=/var/www/uploads
FILE_NAME=$(ls -1rt $FILE_PATH | head -1)
cd /mnt/nand1-2 && killall watch_dog && fuser -k *
cd $FILE_PATH && mv $FILE_NAME /mnt/nand1-2/$UPG_FILE
cd /mnt/nand1-2/ && upgrade -d $UPG_FILE -C /tmp                      
cd /tmp && chmod +x upgrade.sh && ./upgrade.sh  
/sbin/reboot
