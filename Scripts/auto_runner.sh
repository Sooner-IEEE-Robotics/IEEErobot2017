#!/bin/bash
# Robot
#
# description: run program as startup

case $1 in
    start)
        /bin/bash /root/startup.sh
    ;;
esac
exit 0