# OurScheme – Project 1

A simplified Scheme interpreter (2017 Spring PL Project 1).  
This project implements **S-expression (S-exp) parsing and pretty-printing** with error handling.

---

## Features
- **Interactive system** (similar to Petite Chez Scheme).
- Reads input S-expressions and prints them in formatted style.
- Supports **atoms, lists, dotted pairs, quotes, strings, symbols**.
- Handles **escape characters** in strings (`\n`, `\t`, `\"`, `\\`).
- Supports **comments** with `;` (ignored until end of line).
- Built-in constants:
  - `t`, `#t` → always printed as `#t`
  - `nil`, `()`, `#f` → always printed as `nil`

---

## Input / Output Behavior
1. Starts with:
   ```
   Welcome to OurScheme!
   ```
2. Prompts with `> ` and waits for input.
3. Prints S-expressions in pretty format:
   ```scheme
   > (1 . (2 . (3 . 4)))
   ( 1
     2
     3
     .
     4
   )
   ```
4. Quoted input `'(...)` is transformed into `(quote ...)`.
   ```scheme
   > '(1 2 3)
   ( quote
     ( 1
       2
       3
     )
   )
   ```
5. Terminates when `(exit)` is entered:
   ```
   Thanks for using OurScheme!
   ```

---

## Error Handling
Four possible error types:

- `ERROR (unexpected token) : atom or '(' expected when token at Line X Column Y is >>...<<`
- `ERROR (unexpected token) : ')' expected when token at Line X Column Y is >>...<<`
- `ERROR (no closing quote) : END-OF-LINE encountered at Line X Column Y`
- `ERROR (no more input) : END-OF-FILE encountered`

⚠ Once an error occurs, the entire line is skipped and the system continues from the next line.

---

## Printing Rules
- Atoms: printed directly with `\n`.
  - `#t` always printed as `#t`
  - `nil`, `()`, `#f` always printed as `nil`
  - Floats always 3 decimal places (`3.0 → 3.000`)
- Lists / dotted pairs:
  - Indented format with one space after `(` and aligned sub-expressions.
  - `(1 . (2 . (3 . nil)))` prints as:
    ```scheme
    ( 1
      2
      3
    )
    ```

---

## Notes
- Supports multiple inputs on one line:  
  `(1 2) (3 4) 5`
- Respects **longest match** in tokenizing (e.g., `How` is one token, not `H` + `ow`).
- Input file is tested by **PAL**.
- After `(exit)` is processed, all following input is ignored.

---

👉 This is only **Project 1** (parsing + printing). **Evaluation** (`quote`, `cons`, etc.) comes in later projects.
