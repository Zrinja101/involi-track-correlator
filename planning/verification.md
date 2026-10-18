# Verification Plan

**Parent epic:** [Production Track Correlator](epic.md)

## Verification gates

| Gate | Evidence | Required result |
|---|---|---|
| Configure/build | CMake and build logs | Successful Debug and Release builds with bounded jobs |
| Unit tests | CTest output | All Catch2 tests pass |
| Integration | Executable fixture results | Correct JSONL schema and deterministic behavior |
| Contract | `tools/score.py` | Zero contract errors |
| Scoring | Sample and adversarial fixtures | Epic association, switch, and invalid-report thresholds |
| Coverage | Line/branch report | 100% production-code coverage, exclusions documented |
| Static quality | clang-format, clang-tidy, compiler diagnostics | No unresolved findings |
| Documentation | Doxygen output/log | Successful generation and inspection |
| Memory/performance | Long-stream test evidence | Bounded state and acceptable runtime |
| Container | Docker build/test logs | Unmodified Dockerfile builds and tests successfully |

## Command matrix

Commands will be finalized once the implementation layout is established. The expected commands are:

```sh
cmake -S . -B build/Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/Debug -j10
ctest --test-dir build/Debug --output-on-failure
python3 tools/score.py data/sample --run "./build/Debug/correlator"
```

Container verification:

```sh
docker build -t correlator .
docker run --rm -i correlator < data/sample/reports.jsonl > output.jsonl
python3 tools/score.py data/sample output.jsonl
```

## Test categories

### Unit tests

- Report parsing and validation.
- Geographic calculations and numerical boundaries.
- Receiver-clock evidence.
- Candidate scoring and deterministic tie handling.
- Track lifecycle and bounded retention.
- Scheduler deadlines and monotonic output time.
- JSON serialization.

### Integration and contract tests

- Empty input and one-report input.
- Multiple reports with out-of-order decision opportunities.
- Exact-once answers and output-index validation.
- `arr` and `arr + 2` boundaries.
- End-of-input flushing.
- Malformed and rejected reports.

### Adversarial/regression tests

- Unsynchronized receiver clocks.
- Variable network delays and late bursts.
- Missing, changing, and duplicate identifiers.
- Close aircraft and crossing trajectories.
- Temporary loss of observation.
- Corrupted positions and implausible jumps.
- Long streams for memory behavior.

## Calibration and fixture evidence

Calibration runs must use the fixture families and holdout procedure defined in [ADR-003](decisions/adr-003-calibration-and-fixture-strategy.md). Each run must record the parameter set, fixture revision, scorer metrics, contract errors, runtime, and peak state.

## Completed milestone evidence

- **S1–S9:** Unit and static-analysis results are recorded in each story completion record.
- **S10 commit:** `0b6e3e5` integrated the end-to-end pipeline.
- **S10 unit/CLI verification:** 39 test cases, 148 assertions, and the executable CLI contract test passed.
- **S10 sample scorer:** 63.21% association, 2 ID switches, 4 merged tracks, 1 invalid accepted report out of 15, and 0 contract errors; overall `FAIL` against the 99% association and merged-track targets.
- **F01 calibration:** 10-report single-aircraft fixture passes with 100.00% association, 0 switches, 0 merged tracks, 0 invalid accepted reports, and 0 contract errors.
- **F02 calibration:** 12-report, three-receiver clock-offset fixture passes with 100.00% association, 0 switches, 0 merged tracks, 0 invalid accepted reports, and 0 contract errors.
- **F03 calibration:** 24-report identifier-change and duplicate-identifier fixture passes with 100.00% association, 0 switches, 0 merged tracks, 0 invalid accepted reports, and 0 contract errors.
- **F04 calibration:** Five-report gap/retirement fixture passes with 100.00% association, 1 expected retirement switch, 0 merged tracks, 0 invalid accepted reports, and 0 contract errors.
- **F05 calibration:** After tightening the default distance gate to 500 m and retaining only exact-tie rejection, the 24-report close-crossing/duplicate-ID fixture passes with 100.00% association, 0 merged tracks, 0 switches, and 0 contract errors.
- **Sample regression after F05 fix:** 84.58% association, 122 ID switches, 4 merged tracks, 0 invalid accepted reports, and 0 contract errors; still below the epic target.
- **F06 calibration:** After contradictory identity/position rejection, the eight-report corruption fixture passes with 100.00% valid-report association, 0 invalid accepted reports, and 0 contract errors.
- **Sample regression after F06 fix:** 84.81% association, 121 ID switches, 3 merged tracks, 0 invalid accepted reports, and 0 contract errors; still below the epic target.
- **Not yet verified:** F07–F08 calibration fixtures, 100% line/branch coverage, Doxygen, long-stream memory/performance, Docker build, and final release readiness.

## Coverage policy

Measure coverage for production code only. Generated dependency code, external Catch2 code, and intentionally excluded integration shims must not be counted, and every exclusion must be documented. The target is 100% line and branch coverage. Coverage is evidence of exercised paths, not a substitute for meaningful behavioral assertions.

## Review checklist

- [ ] Requirements trace to implementation and tests.
- [ ] All stories link to this verification plan.
- [ ] No wall-clock sleeps or unbounded containers appear in production paths.
- [ ] Numeric calculations handle invalid values, overflow, and boundary conditions.
- [ ] Track names and output indices are deterministic and unique as required.
- [ ] No diagnostic output corrupts stdout JSONL.
- [ ] Doxygen covers public APIs and non-obvious algorithms.
- [ ] README commands are current.
- [ ] Docker verification is run from an environment with Rancher Desktop Docker access.
