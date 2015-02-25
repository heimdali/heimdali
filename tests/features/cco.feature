Feature: cco

    Scenario: Invoke cco
        When I run the command: cco --help
        Then I see the line in standard output: Change pixel type of image

    Scenario: Convert to floatting point
        # lena is coded on 1 byte
        Given I am in the heimdali data directory
        When I run the command: par lena.inr
        Then I see the standard output:
            """
            lena.inr -x 256	-y 256	-o 1	-f
            ""
            """
        # Values are between 0.117 and 0.937
        When I run the command: ical $HEIMDALI_DATA_DIR/lena.inr
        Then I see the standard output:
        """
        +1.1764706E-01 +4.9966636E-01 +9.3725491E-01
        ""
        """
