Feature: sc

    Scenario: Invoke sc
        When I run the command: sc --help
        Then I see the line in standard output: Multiply each pixel by NUMBER

    Scenario: Multiply each pixel by <factor>
        When I run the command: sc -n <factor> -i $HEIMDALI_DATA_DIR/<image>.<ext> -o <image>_sc<factor>.<ext>
        Then the HDF5 files <image>_sc<factor>.<ext> and $HEIMDALI_DATA_DIR/<image>_sc<factor>.<ext> are equal

    Examples:
    | image              | ext   | factor |
    | imtest_z5_y4_x3_c2 | h5    | 1.5    |
