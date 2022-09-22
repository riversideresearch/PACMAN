#!/bin/sh

cd ..

git clone https://gitlab.special-circumstanc.es/hammer/hammer.git

sudo apt-get install -y gcc make scons pkg-config glib-2.0 glib-2.0-dev

cd hammer

scons

sudo scons install

LD_LIBRARY_PATH=/usr/local/lib/
export LD_LIBRARY_PATH

cd ../PACMAN
