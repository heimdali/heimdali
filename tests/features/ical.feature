Feature: The 'ical' command line tool.

    Scenario: Invoke ical
        When I run the command: ical --help
        Then I see the line in standard output: Compute min, max and mean of images
