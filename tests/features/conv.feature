Feature: conv

    Scenario: Invoke conv
        When I run the command: conv --help
        Then I see the line in standard output: Convolution of image with kernel of any size

    #Scenario: Apply kernel
        #When I run the command: conv $HEIMDALI_DATA_DIR/cossin_z5_y5_x5_c2_r4.inr $HEIMDALI_DATA_DIR/kernel_3_3.inr cossin_z5_y5_x5_c2_r4_conv.inr
        #Then images cossin_z5_y5_x5_c2_r4_conv.inr and $HEIMDALI_DATA_DIR/cossin_z5_y5_x5_c2_r4_conv.inr are equal
