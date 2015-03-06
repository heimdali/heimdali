Feature: fixflo

    Scenario: Invoke fixflo
        When I run the command: fixflo --help
        Then I see the line in standard output: Converts from fixed point to floating point

    Scenario: Convert from unsigned char
        When I run the command: fixflo 0 127 255
        Then I see the standard output:
            """
            "0.000000 0.498039 1.000000 "
            ""
            """

    Scenario: Convert from signed char
        When I run the command: fixflo -s -- -128 0 127
        Then I see the standard output:
            """
            "-1.000000 0.000000 1.000000 "
            ""
            """
            
    Scenario: Convert from 2 signed bits
        When I run the command: fixflo -b 2 -s -- -2 0 1
        Then I see the standard output:
            """
            "-1.000000 0.000000 1.000000 "
            ""
            """

    #Scenario: Exponent value is taking into accout
        #When I run the command: fixflo -f -e 1 -- 255
        #Then I see the standard output:
            #"""
            #"2.000000 "
            #""
            #"""

    #Scenario: Exponent value is taking into accout
        #When I run the command: fixflo -f -e -1 -- 255
        #Then I see the standard output:
            #"""
            #"0.500000 "
            #""
            #"""
