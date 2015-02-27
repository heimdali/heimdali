Feature: cco

    Scenario: Invoke cco
        When I run the command: cco --help
        Then I see the line in standard output: Change pixel type of image

    Scenario: Convert to floating point
        # lena image is coded on fixed point (unsigned char)
        When I run the command: par $HEIMDALI_DATA_DIR/lena_f1.<ext>
        Then I see the standard output:
            """
            $HEIMDALI_DATA_DIR/lena_f1.<ext> -x 256	-y 256	-o 1	-f
            ""
            """
        # Values are between 0.117 and 0.937
        When I run the command: ical $HEIMDALI_DATA_DIR/lena_f1.<ext>
        Then I see the standard output:
        """
        +1.1764706E-01 +4.9966636E-01 +9.3725491E-01
        ""
        """

        # Convert to floating point (float)
        When I run the command: cco -o 4 -r $HEIMDALI_DATA_DIR/lena_f1.<ext> lena_r4.<ext>
        When I run the command: par lena_r4.<ext>
        Then I see the standard output:
            """
            lena_r4.<ext> -x 256	-y 256	-o 4	-r
            ""
            """
        When I run the command: ical lena_r4.<ext>
        Then I see the standard output:
        """
        +1.1764706E-01 +4.9966636E-01 +9.3725491E-01
        ""
        """

        # Convert to back to fixed point (unsigned char)
        When I run the command: cco -o 1 -f $HEIMDALI_DATA_DIR/lena_f1.<ext> lena_r4.<ext>
        When I run the command: par lena_r4.<ext>
        Then I see the standard output:
            """
            lena_r4.<ext> -x 256	-y 256	-o 1	-f
            ""
            """
        When I run the command: ical lena_r4.<ext>
        Then I see the standard output:
        """
        +1.1764706E-01 +4.9966636E-01 +9.3725491E-01
        ""
        """

        # Values greater than 1 are casted to 1
        #sc -n 8.5 -i $HEIMDALI_DATA_DIR/lena_f1.h5 | cco -r - > lena-85r.h5
        # should be:
        # sc -n 8.5 $HEIMDALI_DATA_DIR/lena_f1.h5 | cco -r > lena-85r.h5

    Examples:
        | ext |
        | h5  |
        | inr |
