Feature: detc

    Scenario: Invoke detc
        When I run the command: detc --help
        Then I see the line in standard output: Detect image contour

    Scenario: Read fixed point image
        When I run the command: detc $HEIMDALI_DATA_DIR/lena_f1.inr -sob | tpr -c -l 10 | head -n1
        Then I see the standard output:
        """
        0 0.0862745 0.258824 0.0862745 0.172549 0.431373 0.0862745 0.0862745 0.0862745 0.172549
        ""
        """

