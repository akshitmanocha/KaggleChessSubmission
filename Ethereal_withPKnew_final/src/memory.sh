echo "memory testing started"

cat << EOF > memory.exp
   set timeout 10
   lassign \$argv pos depth movetime
   spawn /usr/bin/time -v ./Ethereal
   send "setoption name Hash value 1\\n"
   send "setoption name Threads value 1\\n"
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

expect memory.exp startpos 20 200
# Gets to depth 14, 6400kb

rm memory.exp

#Peak RAM usage should be less than 5MB
# Look at Maximum resident set size (kbytes), which should be less than 5120