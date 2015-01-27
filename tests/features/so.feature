Feature: The 'so' command line tool.

    Scenario: Invoke so
        When I run the command: so --help
        Then I see the line in standard output: Substract two images

