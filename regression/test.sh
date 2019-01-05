#!/usr/bin/env bash

rm -rf dimacs
mkdir dimacs
cp ../data/dimacs/original/dimacs.tar.gz dimacs/
cd dimacs
tar -xf dimacs.tar.gz

# exclude graphs that are slow
rm -f brock800*.graph
rm -f C1000.9.graph
rm -f C2000.5.graph
rm -f C2000.9.graph
rm -f C4000.5.graph
rm -f DSJC1000.5.graph
rm -f MANN_a45.graph
rm -f MANN_a81.graph
rm -f keller6.graph
rm -f p_hat1500-1.graph
rm -f san1000.graph
cd ../

rm -f log.0901
for file in `ls -1 dimacs/*.graph`; do
    base=`basename $file`
    noext=`echo $base | sed -e "s/\.graph//g"`

    theweight=`cat targets | grep $noext | awk '{print $2}'`

    echo "Testing $noext..."
    ../bin/pls --target-weight=$theweight --algorithm=clique --random-seed=0 --input-file=$file | grep -E "^graph-name|^mc|^selections|^best-solution" | tee -a log.0901
    echo "" | tee -a log.0901

done

diff log.baseline log.0901
exit $?
