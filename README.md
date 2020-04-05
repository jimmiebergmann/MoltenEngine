# Curse Engine
![version](https://img.shields.io/badge/Version-v0.1.0-blue)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/7e21efca9d7d43d9806c39b95a076e9b)](https://app.codacy.com/manual/jimmiebergmann/CurseEngine?utm_source=github.com&utm_medium=referral&utm_content=jimmiebergmann/CurseEngine&utm_campaign=Badge_Grade_Dashboard)
[![Travis CI](https://img.shields.io/travis/jimmiebergmann/CurseEngine/master?label=Travis%20CI)](https://travis-ci.org/jimmiebergmann/CurseEngine)
[![AppVeyor](https://img.shields.io/appveyor/ci/jimmiebergmann/CurseEngine/master?label=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/curseengine/branch/master)
[![Codecov](https://img.shields.io/codecov/c/github/jimmiebergmann/CurseEngine/master?label=Coverage)](https://codecov.io/gh/jimmiebergmann/CurseEngine)
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
See [CMake.md](https://github.com/jimmiebergmann/CurseEngine/blob/master/CMake/CMake.md) for available build options.