Feature: cim

    Scenario: Invoke cim
        When I run the command: cim --help
        Then I see the line in standard output: Inputs ASCII value into image

    Scenario: Handle fixed point and floating point correctly.
        When I run the command: echo <inputs> | cim -x 3 <flags> <filename>
        Then images <filename> and $HEIMDALI_DATA_DIR/<filename> are equal
    Examples:
        | inputs    | flags   | filename              |
        | 0 128 255 | -f -o 1 | zero_half_one_f_o1.h5 |
        | 0 128 255 |         | zero_half_one_f_o1.h5 |
        | 0 .5 1    | -f -o 2 | zero_half_one_f_o2.h5 |
        | 0 .5 1    | -f -o 4 | zero_half_one_f_o4.h5 |
        | 0 .5 1    | -r -o 4 | zero_half_one_r_o4.h5 |
        | 0 .5 1    | -r -o 8 | zero_half_one_r_o8.h5 |

    Scenario: Create a image from stdin (float)
        When I run the command: cat $HEIMDALI_DATA_DIR/zero_to_one.txt | cim -o 4 -r -x 3 -y 4 -z 5 -v 2 zero_to_one_r4_stdin.inr
        The images zero_to_one_r4_stdin.inr and $HEIMDALI_DATA_DIR/zero_to_one_r4.inr are equal

    Scenario: Create a image from a file (float)
        When I run the command: cim -rd $HEIMDALI_DATA_DIR/zero_to_one.txt -o 4 -r -x 3 -y 4 -z 5 -v 2 zero_to_one_r4_file.inr
        The images zero_to_one_r4_file.inr and $HEIMDALI_DATA_DIR/zero_to_one_r4.inr are equal

    Scenario: Create a image from stdin, write to stdout (float)
        When I run the command: cat $HEIMDALI_DATA_DIR/zero_to_one.txt | cim -o 4 -r -x 3 -y 4 -z 5 -v 2 > zero_to_one_r4_stdout.h5
        The images zero_to_one_r4_stdout.h5 and $HEIMDALI_DATA_DIR/zero_to_one_r4.h5 are equal
