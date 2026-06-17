# Biochemical Energy Calculator

Biochemical Energy Calculator is a small C console application for estimating the energy output of complete oxidation of three small organic molecules:

- Glucose
- Glycerol
- Alanine

The program prints a summary table with total heat, total ATP, and total energy loss. It also keeps the calculation logic and presentation logic in separate source files so the project is easy to read and extend.

## Features

- Interactive console menu
- Input validation for menu choices and mol values
- Separate calculation modules for glucose, glycerol, and alanine
- Tabular console output for small-molecule oxidation results
- Windows-friendly build script

## Project Files

- `main.c`: menu, input validation, and routing
- `calc_small.c`: small-molecule energy calculation functions
- `display.c`: console output functions
- `build.bat`: one-click Windows build script
- `bec.exe`: compiled Windows executable
- `GITHUB_PUBLISH_TEMPLATE.md`: reusable publishing template

## Requirements

To build from source, you need:

- A C11-compatible compiler such as `gcc` or `clang`
- A Windows terminal environment if you want to use `build.bat`

## Quick Start

### Option 1: Run the prebuilt executable

If you downloaded the release package, run:

```bat
bec.exe
```

### Option 2: Build from source

Use the build script:

```bat
build.bat
```

Or compile manually:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic main.c calc_small.c display.c -o bec.exe
```

Then run:

```bat
bec.exe
```

## Usage

After launch, the program shows a menu:

- `1` - Calculate glucose
- `2` - Calculate glycerol
- `3` - Calculate alanine
- `0` - Exit safely

After choosing `1`, `2`, or `3`, enter the amount in mol. The program checks that the value is numeric and greater than zero.

## Output Model

The calculation model uses the following values:

- Glucose: modern `32 ATP` model with malate-aspartate shuttle
- Glycerol: `18.5 ATP` per mol
- Alanine: `15 ATP` per mol
- Heat and energy loss are allocated by step according to ATP weight

## Build Script

`build.bat` compiles the project in one step on Windows:

```bat
@echo off
setlocal
gcc -std=c11 -Wall -Wextra -Wpedantic main.c calc_small.c display.c -o bec.exe
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)
echo Build succeeded.
endlocal
```

## GitHub Release Notes Template

Use `GITHUB_PUBLISH_TEMPLATE.md` as a starting point when creating a release or project landing page.

## Example

```text
==== Biochemical Energy Calculator ====
[1] Glucose
[2] Glycerol
[3] Alanine
[0] Exit safely
Enter choice:
```

## Notes

- The executable `bec.exe` is provided for convenience on Windows.
- If you are building on another platform, compile the source files with any C11 compiler.
- For best compatibility, keep the project files in the repository root.
