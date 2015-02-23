Feature: par

    Scenario: Invoke par
        When I run the command: par --help
        Then I see the line in standard output: List image formats

    Scenario: Print all parameters
        Given I am in the heimdali data directory
        When I run the command: par imtest_z5_y4_x3_c2.<ext>
        Then I see the standard output:
            """
            imtest_z5_y4_x3_c2.<ext> -z 5	-y 4	-x 3	-o 4
            ""
            """

    Examples:
      | ext |
      | h5  |
      | inr |

    Scenario: Print one parameter
        Given I am in the heimdali data directory
        When I run the command: par imtest_z5_y4_x3_c2.h5 -x
        Then I see the standard output:
            """
            " -x 3"
            """

    #Scenario: Print image with origin different than 0
        #Given I am in the heimdali data directory
        #When I run the command: par imtest_z5_y4_x3_c2_origin_4_5_6.<ext>
        #Then I see the standard output:
            #"""
            #imtest_z5_y4_x3_c2_origin_4_5_6.<ext> -z 5	-y 4	-x 3	-z0 6	-y0 5	-x0 4	-o 4
            #""
            #"""

    Examples:
      | ext |
      | h5  |
      | inr |
