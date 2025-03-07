#!/bin/bash

make profile-build numa=no
strip -s cfish

7z a -mx=9 cfish.7z cfish
tar cf cfish10.tar main.py cfish.7z

zopfli --i100 cfish10.tar

# 65,536 bytes maximum