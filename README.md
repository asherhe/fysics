# Fysics

A nice and simple 2D rigidbody physics engine.

## Requirements

- CMake 3.22+
- Ninja
- A C++17 compiler
- Git and network access on first configure: the demo fetches [SDL3](https://github.com/libsdl-org/SDL) (`release-3.4.18`) and [Dear ImGui](https://github.com/ocornut/imgui) (`v1.93.0`) via `FetchContent`; the tests fetch [Catch2](https://github.com/catchorg/Catch2) (`v3.8.1`)

## Building

Debug build:

```sh
cmake --preset default
cmake --build --preset default
```

Release build:

```sh
cmake --preset release
cmake --build --preset release
```

Build output goes to `build/<preset>/`. To build only the library and skip fetching SDL3/ImGui, turn off the demo:

```sh
cmake --preset default -DFYSICS_BUILD_DEMO=OFF
```

## Running tests

```sh
ctest --preset default
```

Pass `-DFYSICS_BUILD_TESTS=OFF` at configure time to skip the tests and the Catch2 fetch.

## Running the demo

```sh
./build/default/demo/demo
```

