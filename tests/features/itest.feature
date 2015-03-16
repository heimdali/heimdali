Feature: conv

    Scenario: Invoke itest
        When I run the command: itest --help
        Then I see the line in standard output: Perform simple tests on image informations 

    Scenario: Compare image dimensions
        When I run the command: itest imtest_z5_y4_x3_c2.inr
        Then I see the standard output:
            """
            Image informations are the same.
            ""
            """
