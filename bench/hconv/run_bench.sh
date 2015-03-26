#!/bin/bash

# Parse command line arguments.
if [ "$#" != "3" ]
then
    echo "USAGE: run_bensh CMD IN OUT"
    echo "   for example:"
    echo "   - hconv h5 inr"
    echo "   - hconv inr h5"
    echo "   - inr2h5 inr h5"
    echo "   - h5toinr h5 inr"
    exit 1
fi
CMD=$1
IN=$2
OUT=$3

time_file=${CMD}_${IN}_${OUT}.txt

rm -f $time_file

for input in $(ls imtest_z*_y*_x*_c*.$IN)
do
    output=${input%.$IN}.$OUT
    echo "$CMD $input $output" | tee -a $time_file
    /usr/bin/time \
        --format="real: %E user: %U sys: %S" \
        --output=$time_file \
        --append \
        $CMD $input $output
done
