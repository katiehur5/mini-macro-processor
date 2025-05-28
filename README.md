# TeX-like Macro Processor in C

## Description
This project implements a fully functional, TeX-style macro processor, developed in C for Yale’s CPSC 323: Systems Programming & Computer Organization. It  features a custom parser and expansion engine capable of recursively evaluating user-defined macros, conditionals, file inclusion, and delayed execution via \expandafter.

### The processor supports:
- Custom macro definitions (\def) with argument interpolation and nested expansion
- Recursive evaluation strategy with fine-grained control over execution order
- Built-in control macros: \ifdef, \if, \undef, \include, \expandafter
- Comment stripping and escape character handling
- Error detection for malformed or invalid macro calls
- Efficient memory handling and performance optimization under C11 standards
- Robust testing with edge-case scenarios and Zoo machine compatibility

## Reflection:
This project marked an early milestone in the course and served as a rigorous introduction to systems-level programming. It tested my proficiency in C and low-level memory management, but also my resilience in iterative development through cognitive burnout. In the final stages of implementation, I encountered a natural decline in momentum and incurred a degree of technical debt, which manifested in inconsistent coding patterns and architectural drift caused by my reactive, mid-stream refactoring. These challenges taught me the importance of maintaining code cohesion, adhering to design discipline, and managing burnout within the software development lifecycle.
