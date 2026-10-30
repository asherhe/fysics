# Fysics

2D rigidbody physics engine: a C++17 static library (`fysics`, alias `Fysics::core`) with an SDL3 + ImGui demo app.

## Commands

```sh
cmake --preset default          # configure (Ninja, Debug) -> build/default
cmake --build --preset default  # build
./build/default/demo/demo       # run demo
```

Use the `release` preset for Release. `-DFYSICS_BUILD_DEMO=OFF` skips the demo and its SDL3/ImGui fetch. There are no tests yet.

## Layout

- `include/fysics/` public headers
- `src/` library sources
- `demo/` demo executable
