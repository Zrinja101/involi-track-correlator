#!/usr/bin/env python3
"""Verify that an implausible associated report does not mutate track history."""

import json
import pathlib
import subprocess
import sys


REPORTS = """\
{"arr": 0.0, "rx": "R1", "t": 0.0, "lat": 47.0, "lon": 7.0}
{"arr": 1.0, "rx": "R1", "t": 1.0, "lat": 47.001, "lon": 7.0}
{"arr": 2.0, "rx": "R1", "t": 1.1, "lat": 47.006, "lon": 7.0}
{"arr": 3.0, "rx": "R1", "t": 2.0, "lat": 47.002, "lon": 7.0}
"""


def main() -> int:
    executable = pathlib.Path(sys.argv[1])
    result = subprocess.run(
        [executable], input=REPORTS, text=True, capture_output=True, check=True
    )
    answers = [json.loads(line) for line in result.stdout.splitlines()]
    assert [answer["report"] for answer in answers] == [0, 1, 2, 3], answers
    assert answers[0]["track"] == "T0", answers
    assert answers[1]["track"] == "T0", answers
    assert answers[2]["track"] is None, answers
    assert answers[3]["track"] == "T0", answers
    assert "implausible" in result.stderr, result.stderr
    return 0


if __name__ == "__main__":
    raise SystemExit(main())