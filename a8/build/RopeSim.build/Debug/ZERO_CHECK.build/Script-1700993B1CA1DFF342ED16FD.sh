#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/YZY/cg101/a/a8/build
  make -f /Users/YZY/cg101/a/a8/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/YZY/cg101/a/a8/build
  make -f /Users/YZY/cg101/a/a8/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/YZY/cg101/a/a8/build
  make -f /Users/YZY/cg101/a/a8/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/YZY/cg101/a/a8/build
  make -f /Users/YZY/cg101/a/a8/build/CMakeScripts/ReRunCMake.make
fi

