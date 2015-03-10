Feature: asm

    Scenario: Invoke asm
        When I run the command: asm --help
        Then I see the line in standard output: Assemble 2D images into one 3D image

    Scenario: Assemble 5 images
        When I run the command: asm $HEIMDALI_DATA_DIR/imtest_iz*_z5_y4_x3_c2.h5 imtest_z5_y4_x3_c2.h5 
        Then images imtest_z5_y4_x3_c2.h5 and $HEIMDALI_DATA_DIR/imtest_z5_y4_x3_c2.h5 are equal

