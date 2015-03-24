Feature: regionreader

    Scenario: Invoke regionreader
        When I run the command: regionreader --help
        Then I see the line in standard output: Print subregions to iterate on

    Scenario: Read a whole file in one step
        When I run the command: regionreader -sz 5 -sy 5
        Then I see the standard output:
        """
        0 0 5 5
        ""
        """

    Scenario: Iterate one plane line by line
        When I run the command: regionreader -sz 1 -sy 4 -s 1
        Then I see the standard output:
        """
        0 0 1 1
        0 1 1 1
        0 2 1 1
        0 3 1 1
        ""
        """

    Scenario: Iterate two planes line by line
        When I run the command: regionreader -sz 2 -sy 4 -s 1
        Then I see the standard output:
        """
        0 0 1 1
        0 1 1 1
        0 2 1 1
        0 3 1 1
        1 0 1 1
        1 1 1 1
        1 2 1 1
        1 3 1 1
        ""
        """

    Scenario: Iterate two planes with 2 lines
        When I run the command: regionreader -sz 2 -sy 4 -s 2
        Then I see the standard output:
        """
        0 0 1 2
        0 2 1 2
        1 0 1 2
        1 2 1 2
        ""
        """

    Scenario: Iterate two planes with 3 lines
        When I run the command: regionreader -sz 2 -sy 4 -s 3
        Then I see the standard output:
        """
        0 0 1 3
        0 3 1 1
        1 0 1 3
        1 3 1 1
        ""
        """

    Scenario: Iterate four planes plane by plane
        When I run the command: regionreader -sz 4 -sy 3 -s 3
        Then I see the standard output:
        """
        0 0 1 3
        1 0 1 3
        2 0 1 3
        3 0 1 3
        ""
        """

    Scenario: Iterate four planes with 2 planes
        When I run the command: regionreader -sz 4 -sy 3 -s 6
        Then I see the standard output:
        """
        0 0 2 3
        2 0 2 3
        ""
        """

    Scenario: Iterate planes with a the number of lines in one plane + 1
        When I run the command: regionreader -sz 4 -sy 3 -s 4
        Then I see the standard output:
        """
        0 0 1 3
        1 0 1 3
        2 0 1 3
        3 0 1 3
        ""
        """

    Scenario: Iterate planes with a the number of lines in one plane - 1
        When I run the command: regionreader -sz 4 -sy 3 -s 4
        Then I see the standard output:
        """
        0 0 1 3
        1 0 1 3
        2 0 1 3
        3 0 1 3
        ""
        """
