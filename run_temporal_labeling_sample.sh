#! /bin/bash

echo -n "Building open-pls..."
make 2>&1 > /dev/null
echo "Done"


echo -n "Decompressing data..."
cd data
tar -xzf temporal-labeling.tar.gz
cd temporal-labeling
data_dir=`pwd`
echo "Done"

echo "Running experiments..."
cd ../../
cd experiments/labeling/
./labeling.sh $data_dir
