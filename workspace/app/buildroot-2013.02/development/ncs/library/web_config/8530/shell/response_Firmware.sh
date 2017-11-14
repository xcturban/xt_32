TARGET_DIR=/mnt/nand1-2/www/uploads 
TARGET_FILE=UPDATE_FIREWARE.bin 
TEMP_DIR=/tmp/ 
cd $TARGET_DIR 
FILE_NAME=$(ls -1rt | head -1) 
echo "--------response_Fireware.sh ----------->" 
if [ "$FILE_NAME" ] ; then
	mv $FILE_NAME $TARGET_FILE
fi 
/sbin/reboot
