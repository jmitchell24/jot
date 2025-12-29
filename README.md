
<p align="center">
  <img src="/img/icon-128.png" />
</p>


# jot, a teeny little program that types (datecodes)!

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE.txt)
![Last Commit](https://img.shields.io/github/last-commit/jmitchell24/jot)
![Build Status](https://github.com/jmitchell24/jot/actions/workflows/ci.yml/badge.svg)
![Repo Size](https://img.shields.io/github/repo-size/jmitchell24/jot)

[![GitHub Release](https://img.shields.io/github/v/release/jmitchell24/jot?include_prereleases)](https://github.com/jmitchell24/jot/releases)
[![Issues](https://img.shields.io/github/issues/jmitchell24/jot)](https://github.com/jmitchell24/jot/issues)
[![Stars](https://img.shields.io/github/stars/jmitchell24/jot)](https://github.com/jmitchell24/jot/stargazers)

**Need to type something on your linux machine but don't want to use your fingers, toes, or neural link? Try _jot_!**

## Features

- **It types date codes by default!**
- **If you ask it nicely it'll try to type your strings too**
- **Other Neat Things! Later... Maybe...**

## Build Instructions

```bash
# Clone the repository
git clone https://github.com/jmitchell24/jot.git

# Navigate to the project directory
cd jot

# Make build directory
mkdir build
cd build 

# Generate build files / build project  
cmake .. 
cmake --build . 

```

## Usage

### Basic Usage

```bash
jot
```

### Examples

```bash
# Type a date code like '2025-12-22' (very useful feature)
jot 

# Type something less useful than a datecode 
jot not-a-date-code

# Show help 
jot --help 

# Show version 
jot --version
```


## Roadmap

- [x] Type my datecodes for me 
- [ ] Everything else

## License

This project is licensed under the MIT License.

---

Made by [James Peter Mitchell](https://jpmitchell.ca)