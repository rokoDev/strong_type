[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit&logoColor=white)](https://github.com/pre-commit/pre-commit)

| **`Windows`** | **`Linux`** |
|-------------|-------------|
[![Build status](https://ci.appveyor.com/api/projects/status/81er2p9hhjvjkoca/branch/master?svg=true)](https://ci.appveyor.com/project/rokoDev/strong-type/branch/master)|[![CircleCI](https://circleci.com/gh/rokoDev/strong_type/tree/master.svg?style=svg)](https://circleci.com/gh/rokoDev/strong_type/tree/master)|

# strong_type

## Building for Xcode

### Prerequisites:
 - Installed CMake with version 3.18.3 or higher.
 - Installed Xcode.

To generate Xcode project run this command sequence in terminal:
```
git clone https://github.com/rokoDev/strong_type.git
cd strong_type
cmake --preset=xcode
```

Then run this command to open Xcode project:
```
cmake --open ./
```

In Xcode choose `strong_type_tests` target and hit `command+R` to run unit tests.

## If you are going to commit:
Install `pre-commit` package. For instructions see: [pre-commit installation](https://pre-commit.com/#install)

In order to be able to make commits with proper formatting and other checks you should run this commands after clonning the repo:
  1. `cd strong_type`
  2. `pre-commit install`
