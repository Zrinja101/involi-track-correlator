#!/usr/bin/env python3
"""Exercise the correlator's stdin/stdout JSONL contract."""

import json
import pathlib
import subprocess
import sys


REPORTS = """\
{"arr": 10.0, "rx": "R1", "t": 9.5, "lat": 47.0, "lon": 7.0}
{"arr": 10.5, "rx": "R2", "t": 10.0, "lat": 47.001, "lon": 7.001}
"""


def main() -> int:
    executable = pathlib.Path(sys.argv[1])
    result = subprocess.run(
        [executable], input=REPORTS, text=True, capture_output=True, check=True
    )
    reports = [json.loads(line) for line in REPORTS.splitlines()]
    answers = [json.loads(line) for line in result.stdout.splitlines()]
    assert len(answers) == 2, answers
    assert [answer["report"] for answer in answers] == [0, 1], answers
    assert [answer["track"] for answer in answers] == ["T0", "T0"], answers
    assert all(
        report["arr"] <= answer["at"] <= report["arr"] + 2.0
        for report, answer in zip(reports, answers)
    )
    assert answers[0]["at"] <= answers[1]["at"], answers
    assert result.stderr == "", result.stderr
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
