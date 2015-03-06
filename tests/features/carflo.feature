Feature: carflo

    Scenario: Invoke carflo
        When I run the command: carflo --help
        Then I see the line in standard output: Converts from fixed point (unsigned char) to floating point

    Scenario: Convert fixed point to floating point
        When I run the command: carflo 0 1 2 100 255
        Then I see the standard output:
            """
            "0.000000 0.003922 0.007843 0.392157 1.000000 "
            ""
            """
