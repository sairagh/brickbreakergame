#!/bin/bash
echo "# brickbreakergame" >> README.md
git init
git add *
git commit -m "first commit"
git remote add origin https://github.com/sairagh/brickbreakergame.git
git push -u origin master
