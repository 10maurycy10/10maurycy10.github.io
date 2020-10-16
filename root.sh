echo L337 l1NUx 3xPl107
cd /tmp/
cat > ex.c << EOF
int getuid(){return 0;}
int getgid(){return 0;}
int getegid(){return 0;}
int geteuid(){return 0;}
EOF
gcc -shared ex.c -oex.so
LD_PRELOAD=/tmp/ex.so sh
rm /tmp/ex.c /tmp/ex.so
