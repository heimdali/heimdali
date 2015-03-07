Feature: mh

    Scenario: Invoke mh
        When I run the command: mh --help
        Then I see the line in standard output: Set image values to 1 if above THRESHOLD and 0 if below

    Scenario: Threshold an image
        When I run the command: mh -n 0.5 $HEIMDALI_DATA_DIR/cossin_z5_y5_x5_c2_r4.<ext> cossin_z5_y5_x5_c2_r4_mh_0_5_f1.<ext>
        Then images cossin_z5_y5_x5_c2_r4_mh_0_5_f1.<ext> and $HEIMDALI_DATA_DIR/cossin_z5_y5_x5_c2_r4_mh_0_5_f1.<ext> are equal
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Read from standard input and write to standard output
        When I run the command: cat $HEIMDALI_DATA_DIR/cossin_z5_y5_x5_c2_r4.h5 | mh -n 0.5 > cossin_z5_y5_x5_c2_r4_mh_0_5_f1_sdtin.h5
        Then images cossin_z5_y5_x5_c2_r4_mh_0_5_f1_sdtin.h5 and $HEIMDALI_DATA_DIR/cossin_z5_y5_x5_c2_r4_mh_0_5_f1.h5 are equal
