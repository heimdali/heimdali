Feature: cim

    Scenario: Invoke cim
        When I run the command: cim --help
        Then I see the line in standard output: Inputs ASCII value into image

    Scenario: Create a image from stdin (float)
        When I run the command: cat $HEIMDALI_DATA_DIR/zero_to_one.txt | cim -o 4 -r -x 3 -y 4 -z 5 -v 2 zero_to_one_r4_stdin.inr
        The images zero_to_one_r4_stdin.inr and $HEIMDALI_DATA_DIR/zero_to_one_r4.inr are equal

    Scenario: Create a image from a file (float)
        When I run the command: cim -rd $HEIMDALI_DATA_DIR/zero_to_one.txt -o 4 -r -x 3 -y 4 -z 5 -v 2 zero_to_one_r4_file.inr
        The images zero_to_one_r4_file.inr and $HEIMDALI_DATA_DIR/zero_to_one_r4.inr are equal

    Scenario: Create a image from stdin, write to stdout (float)
        When I run the command: cat $HEIMDALI_DATA_DIR/zero_to_one.txt | cim -o 4 -r -x 3 -y 4 -z 5 -v 2 > zero_to_one_r4_stdout.h5
        The images zero_to_one_r4_stdout.h5 and $HEIMDALI_DATA_DIR/zero_to_one_r4.h5 are equal

    Scenario: Create a image from stdin (defaults to -f -o 1, ie fixed point with char)
        When I run the command: echo "28 56 84" | cim -x 3 0.11_0.22_0.33_f_o1.h5
        The images 0.11_0.22_0.33_f_o1.h5 and $HEIMDALI_DATA_DIR/0.11_0.22_0.33_f_o1.h5 are equal

    Scenario: Create a image from stdin (fixed point with short)
        When I run the command: echo "0.11 0.22 0.33" | cim -x 3 -f -o 2 0.11_0.22_0.33_f_o2.h5
        The images 0.11_0.22_0.33_f_o2.h5 and $HEIMDALI_DATA_DIR/0.11_0.22_0.33_f_o2.h5 are equal

    Scenario: Create a image from stdin (fixed point with int)
        When I run the command: echo "0.11 0.22 0.33" | cim -x 3 -f -o 4 0.11_0.22_0.33_f_o4.h5
        The images 0.11_0.22_0.33_f_o4.h5 and $HEIMDALI_DATA_DIR/0.11_0.22_0.33_f_o4.h5 are equal
