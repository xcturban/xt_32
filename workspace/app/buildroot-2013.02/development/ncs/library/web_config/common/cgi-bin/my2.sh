rm www -r
tftp 192.168.1.100 -gr www.tar.gz
tar xvf www.tar.gz
rm www.tar.gz
cd www/shell
./boa_start.sh
