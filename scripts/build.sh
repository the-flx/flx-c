#!/bin/bash
# ========================================================================
# $File: build.sh $
# $Date: 2024-03-06 16:49:29 $
# $Revision: $
# $Creator: Jen-Chieh Shen $
# $Notice: See LICENSE.txt for modification and distribution information
#                   Copyright © 2024 by Shen, Jen-Chieh $
# ========================================================================

cd ..

echo
echo ::Build ninja
cmake -S . -B build/ninja/ -GNinja

echo
echo ::Build macOS
cmake -S . -B build/macos/


