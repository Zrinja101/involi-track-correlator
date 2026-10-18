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


def generate_f02(output_dir):
    reports = []
    truths = []
    receiver_offsets_secs = {"R1": -1.5, "R2": 2.0, "R3": 0.25}
    receivers = ["R1", "R2", "R3"]
    for index in range(12):
        arr_secs = 200.0 + index * 0.5
        receiver = receivers[index % len(receivers)]
        reports.append(
            {
                "arr": arr_secs,
                "rx": receiver,
                "t": arr_secs + receiver_offsets_secs[receiver],
                "id": "AC02",
                "lat": 47.1 + index * 0.0001,
                "lon": 7.1 + index * 0.0002,
                "alt": 800.0 + index,
            }
        )
        truths.append({"report": index, "aircraft": "AC02"})
    write_jsonl(output_dir / "reports.jsonl", reports)
    write_jsonl(output_dir / "truth.jsonl", truths)


def generate_f03(output_dir):
    reports = []
    truths = []
    aircraft = {
        "AC03": (47.2, 7.2, None),
        "AC04": (47.3, 7.3, "DUP"),
        "AC05": (47.4, 7.4, "DUP"),
    }
    report_index = 0
    for step in range(8):
        for aircraft_name, (latitude_degs, longitude_degs, identifier) in aircraft.items():
            arr_secs = 300.0 + step * 1.0 + report_index * 0.01
            report = {
                "arr": arr_secs,
                "rx": "R1" if report_index % 2 == 0 else "R2",
                "t": arr_secs - 0.5,
                "lat": latitude_degs,
                "lon": longitude_degs + step * 0.0002,
                "alt": 600.0 if aircraft_name == "AC03" else 900.0,
            }
            if aircraft_name == "AC03" and step >= 2:
                report["id"] = "AC03" if step < 5 else "AC03-NEW"
            elif identifier is not None:
                report["id"] = identifier
            reports.append(report)
            truths.append({"report": report_index, "aircraft": aircraft_name})
            report_index += 1
    write_jsonl(output_dir / "reports.jsonl", reports)
    write_jsonl(output_dir / "truth.jsonl", truths)


def generate_f04(output_dir):
    reports = []
    truths = []
    arrival_times_secs = [400.0, 401.0, 430.0, 450.0, 512.0]
    for index, arr_secs in enumerate(arrival_times_secs):
        reports.append(
            {
                "arr": arr_secs,
                "rx": "R1",
                "t": arr_secs - 0.5,
                "id": "AC06",
                "lat": 47.5,
                "lon": 7.5 + (arr_secs - 400.0) * 0.0001,
                "alt": 700.0,
            }
        )
        truths.append({"report": index, "aircraft": "AC06"})
    write_jsonl(output_dir / "reports.jsonl", reports)
    write_jsonl(output_dir / "truth.jsonl", truths)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output_dir", type=pathlib.Path)
    parser.add_argument(
        "--family", choices=("f01", "f02", "f03", "f04"), default="f01"
    )
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)
    if args.family == "f01":
        generate_f01(args.output_dir)
    elif args.family == "f02":
        generate_f02(args.output_dir)
    elif args.family == "f03":
        generate_f03(args.output_dir)
    else:
        generate_f04(args.output_dir)


if __name__ == "__main__":
    main()
