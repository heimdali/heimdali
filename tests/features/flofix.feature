Feature: flofix

    Scenario: Invoke flofix
        When I run the command: flofix --help
        Then I see the line in standard output: Converts from floating point to fixed point

    Scenario: Convert to unsigned char
        When I run the command: flofix -f -- 0 0.5 1
        Then I see the standard output:
            """
            "0 128 255 "
            ""
            """

    Scenario: Convert to signed char
        When I run the command: flofix -s -- -1 -0.5 0 0.5 1
        Then I see the standard output:
            """
            "-128 -64 0 64 127 "
            ""
            """

    Scenario: Exponent value is taking into accout
        When I run the command:  flofix -f -e 1 -- 1
        Then I see the standard output:
            """
            "128 "
            ""
            """

    Scenario: Convert from fixed point (unsigned char) to fixed point (unsigned short)
        When I run the command: flofix -o 2 -- `fixflo -o 1 -- 127 `
        Then I see the standard output:
        """
         "32639 "
         ""
         """
