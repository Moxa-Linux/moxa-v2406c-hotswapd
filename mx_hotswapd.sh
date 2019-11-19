#!/bin/bash

### BEGIN INIT INFO
# Provides:          Start mxhtspd daemon
# Required-Start:    
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Run /usr/bin/mxhtspd if it exist
### END INIT INFO

. /lib/lsb/init-functions
PIDFILE=/var/run/mxhtspd.pid
udevadm trigger
case "$1" in

start)
	echo "Starting mxhtspd daemon..."
	#Sleep due to driver loading delay
	sleep 1
	if [ -f /var/run/mxhtspd.pid ] ; then
		echo "WARNING! mxhtspd is running, remove /var/run/mxhtspd.pid if it terminated"
		exit 0
	fi
	#Add parameter if necessary
	mxhtspd &
	;;
stop)	
	echo "Stopping mxhtspd daemon..."
	killall mxhtspd 
	;;
restart)
	echo "Restarting mxhtspd daemon..."
	killall mxhtspd &> /dev/null
	if [ -f /var/run/mxhtspd.pid ] ; then
		echo "WARNING! mxhtspd is running, remove /var/run/mxhtspd.pid if it is no longer running"
		exit 0
	fi
	mxhtspd &
        ;;
*)	
        exit 2
        ;;
esac

exit 0
