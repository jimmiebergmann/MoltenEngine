# Molten Engine
![version](https://img.shields.io/badge/Version-v0.1.0-blue) [![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT) ![GitHub Workflow Status (branch)](https://img.shields.io/github/workflow/status/jimmiebergmann/MoltenEngine/Build/master?label=Github&logo=Github)  [![AppVeyor](https://img.shields.io/appveyor/ci/jimmiebergmann/MoltenEngine/master?label=AppVeyor&logo=AppVeyor)](https://ci.appveyor.com/project/jimmiebergmann/MoltenEngine/branch/master) [![Codecov code coverage](https://img.shields.io/codecov/c/github/jimmiebergmann/MoltenEngine/master?label=Codecov&logo=Codecov)](https://codecov.io/gh/jimmiebergmann/MoltenEngine) [![Codacy Code Quality](https://app.codacy.com/project/badge/Grade/de11d71e399a4f6bb3e19c9cae6d8f95)](https://www.codacy.com/manual/jimmiebergmann/MoltenEngine?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=jimmiebergmann/MoltenEngine&amp;utm_campaign=Badge_Grade) [![LGTM Code Quality](https://img.shields.io/lgtm/grade/cpp/github/jimmiebergmann/MoltenEngine?label=code%20quality&logo=lgtm&)](https://lgtm.com/projects/g/jimmiebergmann/MoltenEngine/) [![LGTM Total alerts](https://img.shields.io/lgtm/alerts/g/jimmiebergmann/MoltenEngine.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/jimmiebergmann/MoltenEngine/alerts/)

Real-time game engine framework and editor.  
**Warning:** Under heavy development.

## Requirements
* C++ >= 20 compiler
    - Windows: Visual Studio >= 2022 (17.3.0)
    - Linux: G++ >= 11 or Clang++ >= 13
* CMake >= 3.16

## Build
``` shell
git clone --recurse-submodules https://github.com/jimmiebergmann/MoltenEngine.git
mkdir MoltenEngine/Build
cd MoltenEngine/Build
cmake ..
cmake --build . --config Release
```
See [Engine/CMake/README](https://github.com/jimmiebergmann/MoltenEngine/blob/master/Engine/CMake/README.md) for available build options.