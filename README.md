# DSA_Assignment1
Data structures and algorithms assignment 1 (Polynomial, Text Editor, UNO Game).

##  Overview

This repository contains implementations of three data structure problems using linked lists in C++:

1. **Polynomial ADT** - Mathematical polynomial operations
2. **Text Editor Simulation** - Cursor-based text manipulation
3. **UNO Card Game** - Simplified UNO game simulation

---

##  Features

### Problem 1: Polynomial ADT
- ✅ Insert terms with coefficient and exponent
- ✅ Display polynomials in mathematical notation
- ✅ Add two polynomials
- ✅ Multiply two polynomials
- ✅ Calculate derivatives

**Example:**
p1: 3x^4 + 2x^2 - x + 5
p2: x^4 + 1
sum: 4x^4 + 2x^2 - x + 6

### Problem 2: Text Editor
- ✅ Insert characters at cursor position
- ✅ Delete characters (backspace functionality)
- ✅ Move cursor left/right
- ✅ Display text with cursor indicator

**Example:**
After insert 'a': a|
After insert 'b': ab|
After move left: a|b

### Problem 3: UNO Game
- ✅ 2-4 player support
- ✅ 76-card deck with 4 colors
- ✅ Action cards: Skip, Reverse, Draw Two
- ✅ Automatic turn management
- ✅ Win condition detection

**Example:**
Player 0's turn, Direction: Clockwise, Top: Blue 3, Players cards: P0:7, P1:7

---

##  Technologies Used

- **Language:** C++
- **Data Structures:** Singly Linked Lists, Doubly Linked Lists
- **Libraries:** `<string>`, `<vector>`, `<random>`, `<algorithm>`
- **Build System:** g++ compiler

---

## 📂 Project Structure
dsa-assignment1/
├── polynomial.h          # Polynomial class interface
├── polynomial.cpp        # Polynomial implementation
├── texteditor.h          # Text editor class interface
├── texteditor.cpp        # Text editor implementation
├── uno.h                 # UNO game class interface
├── uno.cpp               # UNO game implementation
├── main1.cpp             # Polynomial test cases
├── main2.cpp             # Text editor test cases
├── main3.cpp             # UNO game test cases
└── README.md             # This file

---

## 💻 Compilation & Execution

### Prerequisites
- g++ compiler (supporting C++11 or higher)
- Command line terminal

### Compile & Run

**Polynomial ADT:**
```bash
g++ main1.cpp polynomial.cpp -o polynomial
./polynomial

**Text Editor:**
g++ main2.cpp texteditor.cpp -o texteditor
./texteditor

**UNO Game:**
g++ main3.cpp uno.cpp -o uno
./uno
