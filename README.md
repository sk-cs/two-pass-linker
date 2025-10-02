# Two-Pass Linker

A simulated two-pass linker implementation for Operating Systems class, written in C++. This linker processes object files and generates executable binary code with proper symbol resolution and error handling.

## Academic Performance

- **Test Cases**: 100% (20/20 passed)
- **Final Submission**: 95% (Grade: A)
- **Overall**: Excellent performance demonstrating mastery of linker concepts

## Project Overview

This project implements a two-pass linker that:
- **Pass 1**: Builds a global symbol table by processing all modules
- **Pass 2**: Generates binary code with proper address resolution and error handling

## Features

- ✅ **Symbol Table Generation**: Creates global symbol table with absolute addresses
- ✅ **Module Management**: Tracks module sizes and base addresses
- ✅ **Instruction Processing**: Handles A, E, R, I, M instruction types
- ✅ **Error Detection**: Comprehensive error checking and reporting
- ✅ **Warning System**: Detects unused symbols and multiple definitions
- ✅ **Binary Code Generation**: Produces properly formatted output

## Instruction Types Supported

- **A (Absolute)**: Absolute addressing
- **E (External)**: External symbol references
- **R (Relative)**: Relative addressing within module
- **I (Immediate)**: Immediate value instructions
- **M (Module)**: Module-based addressing

## Error Handling

The linker detects and reports:
- Multiple symbol definitions
- Undefined symbols
- Address out of range errors
- Illegal opcodes and operands
- Unused symbols (warnings)

## How to Compile

```bash
make
```

Or manually:
```bash
g++ -o linker linker.cpp
```

## How to Run

### Basic Usage
```bash
./linker < input-file > output-file
```

### Examples
```bash
# Run on a specific input file
./linker input-1 > my_output.txt

# Compare with expected output
./linker input-1 > my_output.txt
diff actual/out-1 my_output.txt
```

## How to Grade

### Using the Automated Grading Script
```bash
# Run all test cases (1-20) and compare with expected outputs
./gradeit.sh actual/ . LOG.txt

# This will:
# - Compare your outputs with reference outputs in actual/ directory
# - Generate LOG.txt only if there are failures
# - Show final score (out of 100)
```

### Manual Testing
```bash
# Test individual cases
./linker input-1 > out-1
diff actual/out-1 out-1

./linker input-2 > out-2
diff actual/out-2 out-2
# ... and so on
```

## Input Format

The linker expects input files with the following structure:
```
<defcount> <symbol1> <address1> <symbol2> <address2> ... <usecount> <symbol1> <symbol2> ... <codecount> <instruction1> <instruction2> ...
```

## Output Format

The linker produces:
1. **Symbol Table**: Global symbols with their absolute addresses
2. **Binary Code**: Memory addresses and instruction values
3. **Error Messages**: Syntax errors, undefined symbols, etc.
4. **Warnings**: Unused symbols, multiple definitions

## Test Results

- **Test Cases**: 20/20 passed ✅
- **Final Score**: 100/100 (Perfect Score) 🎉
- **Grade**: A (95% on final submission)

## File Structure

```
├── linker.cpp          # Main linker implementation
├── makefile           # Build configuration
├── gradeit.sh          # Automated grading script
├── actual/             # Reference output files
│   ├── out-1
│   ├── out-2
│   └── ...
├── input-1             # Test input files
├── input-2
└── ...
```

## Key Implementation Details

### Pass 1 (Symbol Table Generation)
- Processes each module sequentially
- Builds global symbol table with absolute addresses
- Detects multiple symbol definitions
- Tracks module sizes for base address calculations

### Pass 2 (Binary Code Generation)
- Resolves external symbol references
- Handles relative addressing within modules
- Processes module-based addressing (M instructions)
- Generates error messages for illegal operations

## Error Codes

- `Error: This variable is multiple times defined; first value used`
- `Error: External operand exceeds length of uselist; treated as relative=0`
- `Error: [symbol] is not defined; zero used`
- `Error: Absolute address exceeds machine size; zero used`
- `Error: Relative address exceeds module size; relative zero used`
- `Error: Illegal immediate operand; treated as 999`
- `Error: Illegal module operand; treated as module=0`
- `Error: Illegal opcode; treated as 9999`

## Development Notes

This implementation successfully handles all edge cases and produces correct output for all test scenarios. The linker demonstrates proper understanding of:
- Symbol table management
- Address resolution algorithms
- Error handling and reporting
- Module-based addressing schemes
- Two-pass compilation techniques

