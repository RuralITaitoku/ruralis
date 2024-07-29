set -x



if [ "$1" == "clean" ]; then
    make clean
elif [ "$1" == "all" ]; then
    make all
elif [ "$1" == "check" ]; then
    ps -ax | grep ruralis
elif [ "$1" == "net" ]; then
    netstat -an | grep 158
elif [ "$1" == "run" ]; then
    nohup ./ruralis > log.log 2>&1 &
elif [ "$1" == "log" ]; then
    tail -f log.log
else
    make all
fi