#! /bin/bash

time_out=$2
seeds=$3
seeds_minus_one=$((seeds - 1))

output_dir=results/parallel_labeling.$seeds.$time_out
old_output_dir=results/old.parallel_labeling.$seeds.$time_out
data_dir=$1
csv_dir=$output_dir/csvs


rm -rf $old_output_dir
mv $output_dir  $old_output_dir

split_file_prefix="make_me_unique"
mkdir $output_dir
rm -rf $output_dir/log.*
rm -rf $output_dir/$split_file_prefix*
rm -rf $csv_dir
mkdir $csv_dir

num_cores=`cat /proc/cpuinfo | grep processor | wc -l`
#num_cores="2"
num_lines=`ls -1 $data_dir/ | grep ".*k-1.*\.graph$" | wc -l`
num_lines_per_core=$((num_lines/num_cores+ 1))

echo "num_lines_per_core=$num_lines_per_core"


current_dir=`pwd`
full_data_dir=`realpath $data_dir`
cd $output_dir
ls -1 $full_data_dir/ | grep ".*k-1.*\.graph$" | split --lines=$num_lines_per_core - $split_file_prefix
cd $current_dir
for file in `ls -1 $output_dir/$split_file_prefix*`; do
echo $file
done


echo "SEED,K,AM,PLS MAX SCORE,PLS MAX TIME" > $output_dir/intermediate.csv

function process_split_file () {

    split_file_name=$1

    iter=1
    for file_name in `cat $split_file_name`; do
        iter=$((iter + 1))
        # graph information about data set, AM1 or AM2 or AM3

        data_name=`echo $file_name | sed -e "s/.graph$//g"`
        echo "data_name=$data_name"

        #graphml_file_name=$data_dir/../graphml/$data_name.graphml
        #echo "graphml_file_name=$graphml_file_name"

        node_mapping_file="$data_dir/$file_name.node_mapping"

        am=`echo $file_name | sed -e "s/.*AM\([1-3]\).*/\1/g"`
        echo "am=AM$am"

        prefix=`echo $file_name | sed -e "s/\(.*\)\.graphml.*/\1/g"`
        echo "prefix=$prefix"

        seed=`echo $prefix | sed -e "s/.*seed_\([0-9]*\).*/\1/g"`
        echo "seed=$seed"

        k=`echo $prefix | sed -e "s/.*seed_[0-9]*-k\(-\?[0-9]*\).*/\1/g"`
        echo "k=$k"

        if (( (("$k" == "5" ) || ("$k" == "10")) && ("$am" != "1" ) )); then
            echo "Skipping data set: k=10/5 and am > 1"
            continue
        fi

        log_file=$output_dir/log.$file_name.timeout_$time_out

        #rm -rf $log_file

        echo -n "Running $file_name "
        for random_seed in $(seq 0 $seeds_minus_one); do
            echo -n "$random_seed/$seeds_minus_one..."
            echo "../../bin/pls --algorithm=labeling --input-file=$data_dir/$file_name --weighted --use-weight-file --timeout=$time_out --random-seed=$random_seed >> $log_file"
            ../../bin/pls --algorithm=labeling --no-reduce --input-file=$data_dir/$file_name --weighted --use-weight-file --timeout=$time_out --random-seed=$random_seed >> $log_file
            #echo "greedy-weight: $greedy_weight" >> $log_file
            echo "file-seed: $seed" >> $log_file
            echo "file-k: $k" >> $log_file
            echo "file-am: AM$am" >> $log_file
        done

        echo ""
        echo "Make solution csv..."
        python mk_solution_csv.py $log_file $node_mapping_file > $csv_dir/$data_name.csv

        echo "iter=$iter"
        #exit 1
    done
}


for split_file_name in `ls -1 $output_dir | grep "^$split_file_prefix"`; do

    echo "processing $split_file_name"
    process_split_file $output_dir/$split_file_name&
    #if [ "$iter" -gt "100" ]; then
    #    break;
    #fi



done

# let all jobs finish
wait

echo "Add to table..."
for log_file in `ls -1 $output_dir | grep "^log.*"`; do
    echo "Add to intermediate csv..."
    python csvgen.py $output_dir/$log_file >> $output_dir/intermediate.csv
done

### generate real csv from intermediate csv
python mk_final_csv.py $output_dir/intermediate.csv > $output_dir/final.csv
