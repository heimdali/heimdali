Feature: inrimage

    Scenario: Run the inrimage_read.cxx example
        Given I build the inrimage_read example
        Then I run the example: inrimage_read $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.<ext>
    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Run the inrimage_write.cxx example
        Given I build the inrimage_write example
        When I run the example: inrimage_write h5
        Then images inrimage_write_0.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 are equal
        Then images inrimage_write_1.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 are equal
        Then images inrimage_write_2.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 are equal
