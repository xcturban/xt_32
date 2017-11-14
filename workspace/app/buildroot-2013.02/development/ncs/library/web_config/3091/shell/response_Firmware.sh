echo "--------response_Fireware.sh ----------->" 
TARGET_DIR=/mnt/nand1-2/www/uploads 
TARGET_FILE=UPDATE_FIREWARE.bin 
TEMP_DIR=/var/www/uploads/ 
cd $TEMP_DIR
FILE_NAME=$(ls -1rt | head -1) 
if [ "$FILE_NAME" ] ; then
	mv $FILE_NAME $TARGET_DIR/$TARGET_FILE
fi 
/sbin/reboot
