#!/bin/bash

#5120 KiB max

echo "memory testing started"

cat << EOF > memory.exp
   set timeout 10
   lassign \$argv pos depth movetime
   spawn /usr/bin/time -v ./cfish
   send "position \$pos\\ngo movetime \$movetime depth \$depth\\n"
   expect "bestmove" {} timeout {exit 1}
   send "quit\\n"
   expect eof
EOF

# cat << EOF > memory.exp
#    set timeout 10
#    lassign \$argv pos depth movetime
#    spawn /usr/bin/time -v ./cfish
#    send "setoption name Hash value 1\\n"
#    send "setoption name Threads value 1\\n"
#    send "quit\\n"
#    expect eof
# EOF

# expect memory.exp startpos 1 100
# 5376kb, most of it is probably the hash table

# 6144kb
#expect memory.exp startpos 6 200

expect memory.exp startpos 20 200
# Gets to depth 14, 6400kb

# setting tt size to very small, line 64 count = mbSize * 1024 / sizeof(Cluster); 
# gets to depth 14, 6912kb

# setting enum { MAX_MOVES = 128, MAX_PLY = 16 } in types.h gets to 6780kb;

rm memory.exp

#Peak RAM usage should be less than 5MB
# Look at Maximum resident set size (kbytes), which should be less than 5120