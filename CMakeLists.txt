cmake_minimum_required(VERSION 3.24)
project(correlator CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d)
FetchContent_Declare(Catch2
    URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.7.1.tar.gz
    URL_HASH SHA256=c991b247a1a0d7bb9c39aa35faf0fe9e19764213f28ffba3109388e62ee0269c)
FetchContent_MakeAvailable(json Catch2)

add_library(correlator_core src/arrival_scheduler.cpp src/association_scorer.cpp src/identifier_matcher.cpp src/plausibility_validator.cpp src/report_parser.cpp src/report_validator.cpp src/track_store.cpp)
target_include_directories(correlator_core PUBLIC src)
target_link_libraries(correlator_core PUBLIC nlohmann_json::nlohmann_json)

add_executable(correlator src/main.cpp)
target_link_libraries(correlator PRIVATE correlator_core)

enable_testing()
add_executable(correlator_tests tests/correlator_tests.cpp)
target_link_libraries(correlator_tests PRIVATE correlator_core Catch2::Catch2WithMain)
add_test(NAME correlator_tests COMMAND correlator_tests)
