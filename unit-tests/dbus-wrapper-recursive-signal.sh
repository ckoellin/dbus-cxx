#!/bin/sh

# launch up an instance of the dbus and source the data output
# to see what the address is and what the PID is
TMPFILE=$(mktemp)
dbus-launch > $TMPFILE

. $TMPFILE
export DBUS_SESSION_BUS_ADDRESS
export PATH=$PATH:$(pwd)

./test-recursive-signal A &
SERVER_PID=$!
./test-recursive-signal B
# get the exit code 
EXIT_CODE=$?

# wait for server to be done
wait $SERVER_PID

kill $DBUS_SESSION_BUS_PID
rm $TMPFILE

exit $EXIT_CODE
