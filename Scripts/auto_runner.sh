#!/bin/bash
# Robot
#
# description: run program as startup

case $1 in
    start)
        /bin/bash /root/startup.sh
    ;;
	stop)
        #nothing
    ;;
    restart)
		#nothing
    ;;
	force-stop)
        #nothing
    ;;
    status)
		#nothing
    ;;
esac
exit 0