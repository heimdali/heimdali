Feature: vb

    Scenario: Invoke vb
        When I run the command: vb --help
        Then I see the line in standard output: Set image values to VALUE if values are below THRESHOLD

    Scenario: Threshold an image
        When I run the command: vb -n 0.5 -v 2. $HEIMDALI_DATA_DIR/cossin_z5_y5_x5_c2_r4.<ext> cossin_z5_y5_x5_c2_r4_vb_0_5.<ext>
        Then images cossin_z5_y5_x5_c2_r4_vb_0_5.<ext> and $HEIMDALI_DATA_DIR/cossin_z5_y5_x5_c2_r4_vb_0_5.<ext> are equal
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Read from standard input and write to standard output
        When I run the command: cat $HEIMDALI_DATA_DIR/cossin_z5_y5_x5_c2_r4.h5 | vb -n 0.5 -v 2 > cossin_z5_y5_x5_c2_r4_vb_0_5_sdtin.h5
        Then images cossin_z5_y5_x5_c2_r4_vb_0_5_sdtin.h5 and $HEIMDALI_DATA_DIR/cossin_z5_y5_x5_c2_r4_vb_0_5.h5 are equal
