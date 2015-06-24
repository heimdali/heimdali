Feature: cim

    Scenario: Invoke cim
        When I run the command: cim --help
        Then I see the line in standard output: Inputs ASCII value into image

    Scenario: Create a image from stdin
        When I run the command: cat $HEIMDALI_DATA_DIR/zero_to_one.txt | cim -o 4 -r -x 3 -y 4 -z 5 -v 2 zero_to_one_r4_stdin.inr
        The images zero_to_one_r4_stdin.inr and $HEIMDALI_DATA_DIR/zero_to_one_r4.inr are equal

    Scenario: Create a image from a file
        When I run the command: cim -rd $HEIMDALI_DATA_DIR/zero_to_one.txt -o 4 -r -x 3 -y 4 -z 5 -v 2 zero_to_one_r4_file.inr
        The images zero_to_one_r4_file.inr and $HEIMDALI_DATA_DIR/zero_to_one_r4.inr are equal

    Scenario: Create a image from stdin, write to stdout
        When I run the command: cat $HEIMDALI_DATA_DIR/zero_to_one.txt | cim -o 4 -r -x 3 -y 4 -z 5 -v 2 > zero_to_one_r4_stdout.h5
        The images zero_to_one_r4_stdout.h5 and $HEIMDALI_DATA_DIR/zero_to_one_r4.h5 are equal

    Scenario: Create a image using fixed point
        When I run the command: echo "0.1 0.2 0.3" | cim -x 3 -f cim_fixed_point.h5
        The images cim_fixed_point.h5 and $HEIMDALI_DATA_DIR/cim_fixed_point.h5 are equal

    Scenario: Fixed point (-f) is the default
        When I run the command: echo "0.1 0.2 0.3" | cim -x 3 cim_fixed_point.h5
        The images cim_fixed_point.h5 and $HEIMDALI_DATA_DIR/cim_fixed_point.h5 are equal
