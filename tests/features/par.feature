Feature: The 'par' command line tool.

    Scenario: Invoke par
        When I run the command: par --help
        Then I see the line in standard output: List image formats

    Scenario: Print all values with par
        Given I am in the heimdali data directory
        When I run the command: par imtest_z5_y4_x3_c2.<ext>
        Then I see the standard output:
            """
            imtest_z5_y4_x3_c2.<ext> -z 5 -y 4 -x 3 -z0 0 -y0 0 -x0 0 -o 4
            ""
            """

    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Print one value with par
        Given I am in the heimdali data directory
        When I run the command: par imtest_z5_y4_x3_c2.h5 -x
        Then I see the standard output:
            """
            " -x 3"
            """
