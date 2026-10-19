# Development Guide

This file contains project-development instructions that are intentionally separate from the original repository README.

## Dependencies

- **nlohmann/json v3.11.3:** JSONL report parsing and answer serialization.
- **Catch2 v3.7.1:** C++ unit-test framework.
- **vcpkg:** Installed by the reference Dockerfile for toolchain integration; project dependency versions remain pinned by the CMake `FetchContent` declarations.

## Local build and tests

```sh
cmake -S . -B build/Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/Debug -j10
ctest --test-dir build/Debug --output-on-failure
```

Release verification:

```sh
cmake -S . -B build/Release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release -j10
ctest --test-dir build/Release --output-on-failure
```

## Doxygen

With Doxygen 1.9.6 available on `PATH`:

```sh
cmake -S . -B build/Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/Debug --target doxygen
```

The entry point is:

```text
build/docs/html/index.html
```

## Coverage

```sh
cmake -S . -B build/Coverage -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build/Coverage -j10
ctest --test-dir build/Coverage --output-on-failure
gcovr --root . --object-directory build/Coverage \
  --filter 'src/.*' --txt-metric branch --print-summary
```

Run the synthetic fixtures and sample through `build/Coverage/correlator` before collecting the report so executable paths are included.

The current measured baseline is 93.9% line coverage, 100% function coverage, and 59.6% branch coverage. Improving this baseline is encouraged, but coverage is not a delivery gate.

## Calibration fixtures

Generate a fixture family with:

```sh
python3 tools/generate_calibration_fixtures.py \
  --family f01 data/synthetic/f01-single-aircraft
```

Available families are `f01` through `f08` plus `holdout`. Score a generated fixture with:

```sh
python3 tools/score.py data/synthetic/f01-single-aircraft \
  --run "./build/Debug/correlator"
```

## Container verification

Run these commands from a Docker/Rancher Desktop-connected environment containing the current checkout:

```sh
docker build -t correlator .
docker run --rm -i correlator \
  < data/sample/reports.jsonl > /tmp/correlator-output.jsonl
python3 tools/score.py data/sample /tmp/correlator-output.jsonl
```

The unmodified Dockerfile is the authoritative container build. The final sample regression passes the scorer at 99.85% association, with 18 ID switches, 1 merged track, 0 invalid accepted reports, and 0 contract errors.
