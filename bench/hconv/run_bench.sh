#!/bin/bash

time_file=time.txt

rm -f $time_file

for input in $(ls imtest_z*_y*_x*_c*.h5)
do
    output=${input%.h5}.inr
    echo "hconv $input $output" | tee -a $time_file
    /usr/bin/time \
        --format="real: %E user: %U sys: %S" \
        --output=$time_file \
        --append \
        hconv $input $output
done
