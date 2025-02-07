#!/bin/bash

7z a -mx=9 Ethereal.7z Ethereal
tar cf Ethereal14_pk_full.tar main.py Ethereal.7z pknet_224x32x2.bin
zopfli --i1000 Ethereal14_pk_full.tar

# echo "Compressed binary size zopfli:"
ls -lh Ethereal14_pk_full.tar.gz | awk '{print $5}'