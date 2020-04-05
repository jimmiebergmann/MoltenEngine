# Curse Engine
![version](https://img.shields.io/badge/Version-v0.1.0-blue)
[![Travis CI](https://img.shields.io/travis/jimmiebergmann/CurseEngine/master?label=Travis%20CI)](https://travis-ci.org/jimmiebergmann/CurseEngine)
[![AppVeyor](https://img.shields.io/appveyor/ci/jimmiebergmann/CurseEngine/master?label=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/curseengine/branch/master)
[![Codecov code coverage](https://img.shields.io/codecov/c/github/jimmiebergmann/CurseEngine/master?label=Codecov)](https://codecov.io/gh/jimmiebergmann/CurseEngine)
[![Coveralls code coverage](https://img.shields.io/coveralls/github/jimmiebergmann/CurseEngine/topic-ecs?label=Coveralls)](https://coveralls.io/github/jimmiebergmann/CurseEngine?branch=master)
[![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)

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