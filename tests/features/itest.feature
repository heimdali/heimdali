Feature: conv

    Scenario: Invoke itest
        When I run the command: itest --help
        Then I see the line in standard output: Perform simple tests on image informations 
