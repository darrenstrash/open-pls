#! /bin/bash

time_out=0.1
seeds=5
seeds_minus_one=$((seeds - 1))

output_dir=labeling
data_dir=$1

rm -rf old.$output_dir
mv $output_dir  old.$output_dir
mkdir $output_dir

for file_name in `ls -1 $data_dir/ | grep "\.graph$"`; do

    data_name=`echo $file_name | sed -e "s/-sorted//g" | sed -e "s/.graph//g"`
    target_weight=`cat known.labeling | grep $data_name | awk '{ print $1}'`

    am=`echo $file_name | sed -e "s/.*AM\([1-3]\).*/\1/g"`
    echo "am=AM$am"

    prefix=`echo $file_name | sed -e "s/\(.*\)\.graphml.*/\1/g"`
    echo "prefix=$prefix"

    seed=`echo $prefix | sed -e "s/.*seed_\([0-9]*\).*/\1/g"`
    echo "seed=$seed"

    k=`echo $prefix | sed -e "s/.*seed_[0-9]*-\(.*\)/\1/g"`
    echo "k=$k"

    echo -n "Running $file_name "
    for random_seed in $(seq 0 $seeds_minus_one); do
        log_file=$output_dir/log.$file_name.$random_seed
        echo -n "$random_seed/$seeds_minus_one..."
        #echo "../../bin/pls --algorithm=labeling --input-file=$data_dir/$file_name --target-weight=$target_weight --weighted --use-weight-file --timeout=$time_out --random-seed=$random_seed >> $log_file"
        echo "../../bin/pls --algorithm=labeling --input-file=$data_dir/$file_name --weighted --use-weight-file --timeout=$time_out --random-seed=$random_seed >> $log_file"
        #../../bin/pls --algorithm=labeling --input-file=$data_dir/$file_name --target-weight=$target_weight --weighted --use-weight-file --timeout=$time_out --random-seed=$random_seed >> $log_file
        ../../bin/pls --algorithm=labeling --input-file=$data_dir/$file_name --weighted --use-weight-file --timeout=$time_out --random-seed=$random_seed >> $log_file
        echo "greedy-weight: $greedy_weight" >> $log_file
        echo "file-seed: $seed" >> $log_file
        echo "file-k: $k" >> $log_file
        echo "file-am: AM$am" >> $log_file
    done
done

python tablegen.py
