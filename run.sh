echo "Building..."
gcc -std=c99 -o jufoExecutable main.c testCases.c

echo "Executing..."
echo ""

trap "rm jufo; echo ""; echo \"ERROR: Aborted jufo Program\"; exit" SIGHUP SIGINT SIGTERM
./jufo
rm jufo
