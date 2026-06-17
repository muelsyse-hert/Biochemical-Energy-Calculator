# Biochemical Energy Calculator

## Short Description

Biochemical Energy Calculator is a C console application for estimating ATP yield, heat output, and energy loss during complete oxidation of glucose, glycerol, and alanine.

## What It Does

- Calculates small-molecule oxidation results
- Applies stepwise ATP-weighted heat/loss distribution
- Displays results in a console table
- Includes a Windows build script

## How to Run

### Prebuilt Windows binary

Download `bec.exe` from the release assets and run it from the project folder.

### Build from source

```bat
build.bat
```

Or compile manually:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic main.c calc_small.c display.c -o bec.exe
```

## Usage

1. Start the program.
2. Choose `1`, `2`, or `3`.
3. Enter the amount in mol.
4. View the summary table.
5. Choose `0` to exit safely.

## Repository Layout

- `main.c`
- `calc_small.c`
- `display.c`
- `build.bat`
- `bec.exe`
- `README.md`

## Release Notes Template

### Version 1.0.0

- Added console menu for molecule selection
- Added calculation support for glucose, glycerol, and alanine
- Added formatted result tables
- Added Windows build script

## Tips for Users

- Make sure the amount in mol is greater than zero.
- If you rebuild locally, the executable will be regenerated as `bec.exe`.
- Keep all source files in the repository root for the simplest build process.
