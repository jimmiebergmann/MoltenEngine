# Molten Engine
![version](https://img.shields.io/badge/Version-v0.1.0-blue)
[![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)
[![Travis CI](https://img.shields.io/travis/jimmiebergmann/MoltenEngine/master?label=Travis%20CI)](https://travis-ci.org/jimmiebergmann/MoltenEngine)
[![AppVeyor](https://img.shields.io/appveyor/ci/jimmiebergmann/MoltenEngine/master?label=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/MoltenEngine/branch/master)
[![Codecov code coverage](https://img.shields.io/codecov/c/github/jimmiebergmann/MoltenEngine/master?label=Codecov)](https://codecov.io/gh/jimmiebergmann/MoltenEngine)
[![Coveralls code coverage](https://img.shields.io/coveralls/github/jimmiebergmann/MoltenEngine/topic-ecs?label=Coveralls)](https://coveralls.io/github/jimmiebergmann/MoltenEngine?branch=master)
[![Codacy branch grade](https://img.shields.io/codacy/grade/7e21efca9d7d43d9806c39b95a076e9b/master?label=Code%20Quality)](https://app.codacy.com/manual/jimmiebergmann/MoltenEngine/dashboard)

Real-time game engine framework and editor.  
**Warning:** Under heavy development.

## Build
```sh
$ git clone --recurse-submodules https://github.com/jimmiebergmann/MoltenEngine.git
$ mkdir MoltenEngine/Build
$ cd MoltenEngine/Build
$ cmake ..
$ cmake --build . --config Release
```
See [CMake/README](https://github.com/jimmiebergmann/MoltenEngine/blob/master/CMake/CMake.md) for available build options.