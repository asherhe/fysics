# Fysics

2D rigidbody physics engine: a C++17 static library (`fysics`, alias `Fysics::core`) with an SDL3 + ImGui demo app.

## Commands

```sh
cmake --preset default          # configure (Ninja, Debug) -> build/default
cmake --build --preset default  # build
./build/default/demo/demo       # run demo
ctest --preset default          # run tests (Catch2)
```

Use the `release` preset for Release. `-DFYSICS_BUILD_DEMO=OFF` skips the demo and its SDL3/ImGui fetch; `-DFYSICS_BUILD_TESTS=OFF` skips tests and the Catch2 fetch.

## Layout

- `include/fysics/` public headers
- `src/` library sources
- `demo/` demo executable
- `tests/` Catch2 v3 tests (`fysics_tests`); `src/` is on its include path for internal headers
