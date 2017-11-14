#pid1=`ps -ef |grep linphonec|grep -v grep|wc -l`
pid2=`ps -ef |grep 8530hv|grep -v grep|wc -l`
#if [ $pid1 -lt 1 ] || [ $pid2 -lt 1 ];then
if [ $pid2 -lt 1 ];then
killall -9 8530hv linphonec
8530hv &
sleep 5
#linphonec -C -D -c /root/linphonerc &
fi

