#!/usr/bin/env python3
"""Generate deterministic calibration fixtures for the correlator."""

import argparse
import json
import pathlib


def write_jsonl(path, records):
    path.write_text("".join(json.dumps(record, separators=(",", ":")) + "\n" for record in records))


def generate_f01(output_dir):
    reports = []
    truths = []
    for index in range(10):
        arr_secs = 100.0 + index
        reports.append(
            {
                "arr": arr_secs,
                "rx": "R1" if index % 2 == 0 else "R2",
                "t": arr_secs - (0.5 if index % 2 == 0 else 0.25),
                "id": "AC01",
                "lat": 47.0,
                "lon": 7.0 + index * 0.0005,
                "alt": 500.0,
            }
        )
        truths.append({"report": index, "aircraft": "AC01"})
    write_jsonl(output_dir / "reports.jsonl", reports)
    write_jsonl(output_dir / "truth.jsonl", truths)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output_dir", type=pathlib.Path)
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)
    generate_f01(args.output_dir)


if __name__ == "__main__":
    main()
