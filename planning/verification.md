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
