TESTFOLDER=test_cases_pubblici
OUTDIR=outputs
mkdir -p $OUTDIR
rm -rf $OUTDIR/open*.output.txt
./compile.sh
TIMEFORMAT='%3R'
for i in $(seq 1 11)
do
    TESTFILE=$TESTFOLDER/open$i.txt
    REFFILE=$TESTFOLDER/open$i.output.txt
    OUTFILE=$OUTDIR/open$i.output.txt
    echo "Running $TESTFILE..."
    
    printf "Elapsed time: "
    time ./progetto < "$TESTFILE" > "$OUTFILE"
    printf "Status: "
    if cmp --silent "$REFFILE" "$OUTFILE"; then
        echo "OK"
    else
        echo "FAILED"
        exit 1
    fi
    echo ""
done
