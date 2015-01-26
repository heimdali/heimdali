Feature: The 'par' command line tool.

    Scenario: Invoke par
        When I run the command: par --help
        Then I see the line in standard output: List image formats

    Scenario: Print values with par
        Given I am in directory $HEIMDALI_DATA_DIR
        When I run the command: par imtest_z4_y4_x4_c4.h5
        Then I see the standard output:
            """
            imtest_z5_y4_x3_c2.h5 -z 5 -y 4 -x 3 -z0 0 -y0 0 -x0 0 -o 4
            """
