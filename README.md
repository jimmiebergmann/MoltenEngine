# Curse Engine
![version](https://img.shields.io/badge/Version-v0.1.0-blue)
[![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)
[![Travis CI](https://img.shields.io/travis/jimmiebergmann/CurseEngine/master?label=Travis%20CI)](https://travis-ci.org/jimmiebergmann/CurseEngine)
[![AppVeyor](https://img.shields.io/appveyor/ci/jimmiebergmann/CurseEngine/master?label=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/curseengine/branch/master)
[![Codecov code coverage](https://img.shields.io/codecov/c/github/jimmiebergmann/CurseEngine/master?label=Codecov)](https://codecov.io/gh/jimmiebergmann/CurseEngine)
[![Coveralls code coverage](https://img.shields.io/coveralls/github/jimmiebergmann/CurseEngine/topic-ecs?label=Coveralls)](https://coveralls.io/github/jimmiebergmann/CurseEngine?branch=master)
[![Codacy branch grade](https://img.shields.io/codacy/grade/7e21efca9d7d43d9806c39b95a076e9b/master?label=Code%20Quality)](https://app.codacy.com/manual/jimmiebergmann/CurseEngine/dashboard)

Real-time game engine framework and editor.  
**Warning:** Under heavy development.

## Build
```sh
$ git clone --recurse-submodules https://github.com/jimmiebergmann/CurseEngine.git
$ mkdir CurseEngine/Build
$ cd CurseEngine/Build
$ cmake ..
$ cmake --build . --config Release
```
See [CMake/README](https://github.com/jimmiebergmann/CurseEngine/blob/master/CMake/CMake.md) for available build options.