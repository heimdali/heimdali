Feature: The 'par' command line tool.

    Scenario: Invoke par
        When I run the command: par --help
        Then I see the line in standard output: List image formats

    Scenario: Print values with par
        Given I am in directory $INRIMAGE2_DATA_DIR/test/images
        When I run the command: par imtest_z4_y4_x4_c4.h5
        Then I see the standard output:
            """
            imtest_z4_y4_x4_c4.h5 -z 4 -y 4 -x 4 -z0 0 -y0 0 -x0 0 -o 4
            """

    Scenario: Print values with par
        Given I am in directory $INRIMAGE2_DATA_DIR/test/images
        When I run the command: par imtest_z4_y4_x4_c4.h5
        Then I see the standard output:
            """
            imtest_z4_y4_x4_c4.h5 -z 4 -y 4 -x 4 -z0 0 -y0 0 -x0 0 -o 4
            """
