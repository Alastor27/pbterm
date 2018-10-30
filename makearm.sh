#!/bin/sh

#  This file has been "borrowed" from he example programs from the
#  PocketBook SDK. It is is under the same license as those files
#  are (probably the GPL).

cmake -DCMAKE_BUILD_TYPE=Release -DTARGET_TYPE=ARM
make
