Feature: med

    Scenario: Invoke med
        When I run the command: med --help
        Then I see the line in standard output: Median filter

#   Scenario: Apply median filter
#       When I run the command: med $HEIMDALI_DATA_DIR/lena_r4.h5 lena_med_r4.h5
#       Then images $HEIMDALI_DATA_DIR/lena_med_r4.h5 and lena_med_r4.h5 are equal
