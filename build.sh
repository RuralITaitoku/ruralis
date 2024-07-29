set -x

OS=$(uname);

if [ "$1" = "clean" ]; then
    make clean
elif [ "$1" = "all" ]; then
    make all
elif [ "$1" = "check" ]; then
    set +x
    echo ▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️ ruralisプロセスチェック ▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️
    set -x
    ps -ax | grep ruralis
elif [ "$1" = "net" ]; then
    set +x
    echo ▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️ ネットワークチェック158 ▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️▪️
    set -x
    netstat -an | grep 158
elif [ "$1" = "run" ]; then
    echo $OS
    set +x
    if [ "$OS" = "Darwin" ]; then
        echo ------------------------ MacOSX
        set -x
        nohup ./ruralis > log.log 2>&1 &
        echo "kill $!" > ./ruralis_stop.sh
    else 
        echo ------------------------ $OS
        set -x
        nohup ./ruralis -p 1580 > log.log 2>&1 &
        echo "kill $!" > ./ruralis_stop.sh
    fi
elif [ "$1" = "log" ]; then
    tail -f log.log
elif [ "$1" = "stop" ]; then
    sh ruralis_stop.sh
else
    make all
fi