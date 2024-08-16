
set -x
alias chrome='/Applications/Google\ Chrome.app/Contents/MacOS/Google\ Chrome'


if [ "$1" = "run" ]; then
	chrome lesson1-01.html	
elif [ "$1" = "run2" ]; then
	chrome lesson02.html	
elif [ "$1" = "run3" ]; then
	chrome lesson03.html	
elif [ "$1" = "run4" ]; then
	chrome lesson04.html	
	
else
	npx tsc -p ..
fi
