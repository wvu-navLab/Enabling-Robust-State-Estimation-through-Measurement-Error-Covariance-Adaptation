#!/bin/bash

CURRDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd && cd .. )"
DIR="$(dirname "$CURRDIR")"

TDIR="$DIR/test"
DDIR="$DIR/data"
PDIR="$DIR/examples/make_plots"

export MATLABPATH="$DDIR/truth":"$CURRDIR/make_plots":"$CURRDIR/make_plots/utils"

mkdir -p "$TDIR/plots"
cd "$TDIR/plots"

matlab -r -nodesktop 'make_all_figures'


clear
echo -e "\n\n\n\n ----------------------------------------------- \n"
echo -e " plot done. :-)  All results were written to ../test/plots"
echo -e  "\n ----------------------------------------------- \n\n\n"
