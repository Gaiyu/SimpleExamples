#!/bin/bash
[ -f "./quit.sh" ] && ./quit.sh

./main.py "$1" "$2" &
MAIN_PID=$!
pushd www
python3 -m http.server 8081 &
WEB_SERVER_PID=$!
popd

echo '#!/bin/bash' > quit.sh
echo "kill -9 $MAIN_PID" >> quit.sh
echo "kill -9 $WEB_SERVER_PID" >> quit.sh
echo 'rm $0' >> quit.sh
chmod a+x quit.sh
