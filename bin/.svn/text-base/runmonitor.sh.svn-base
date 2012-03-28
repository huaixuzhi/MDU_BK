#!/bin/sh

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/topway/glib
export LD_LIBRARY_PATH

while [ 1 ];do

a=`/bin/ps -eaf |grep mdu|grep -v grep`
if [ -z "${a}" ];then

cd /topway/topmdu/bin
./mdu&

fi
/bin/sleep 2
done

