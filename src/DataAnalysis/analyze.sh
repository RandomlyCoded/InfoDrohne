#!/bin/bash

OUTDIR=$(date +"%Y-%m-%d_%H-%M")

checkfile() {
    if [[ ! -e "$1" ]];
    then
        echo "no $1!"
        exit 1
    fi
}

checkfile "korad.log"
checkfile "remote.csv"

python merge.py "korad.log" "remote.csv"
python visualize.py "output.csv"

mkdir "$OUTDIR"

mv "korad.log" "remote.csv" "diagram.png" "$OUTDIR/"
mv "output.csv" "$OUTDIR/merged.csv"

echo "finished analyzing $OUTDIR"

vim "$OUTDIR/CHANGELOG.md"
