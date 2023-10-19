# SkipList Dictionary

<br>

#### Repository: [https://github.com/bcmaia/skiplist](https://github.com/bcmaia/skiplist)

#### Students: 
- Bernardo Maia Coelho - 12542481
- Rodrigo de Freitas Lima - 12547510

<br>

This C program provides a command-line interface to perform various operations on a dictionary. This project is a part of the Abstract Data Structures subject, aiming to implement the SkipList data structure in C and demonstrate its usage.


## Overview

A SkipList is a data structure that allows efficient insertion, deletion, and searching of items. This program accepts commands from the user to perform these operations.

## Documentation
We documented the project using Doxygen standards.
You can find the output of Doxygen in the `./doc` folder.

## Code Structure

The program consists of the following main components:

1. **Execution Loop**: The core of the program is an infinite loop that waits for user input and performs various operations based on the provided command.

2. **Operations**:

    - **Insertion**: The user can insert an item into the SkipList using the "insercao" command. The program reads an item, inserts it into the SkipList, and handles any errors.

    - **Update**: The "alteracao" command allows the user to update an existing item in the SkipList. It reads an item, performs the update, and handles errors.

    - **Removal**: The "remocao" command is used to remove an item from the SkipList. It reads an item, removes it from the SkipList, and manages errors.

    - **Search**: The "busca" command allows the user to search for an item in the SkipList. It reads an item, searches for it, and prints the result or handles errors.

    - **Print**: The "impressao" command is used to print the SkipList with different formatting options. The program reads the format specifier and prints the SkipList accordingly, handling errors.

    - **Debug**: The "debug" command prints the current state of the SkipList for debugging purposes.

    - **Operation Not Identified**: If the program encounters an unrecognized command, it outputs "OPERACAO INVALIDA."

3. **Cleanup**: After the user exits the program, it deallocates memory and cleans up the SkipList to prevent memory leaks.

## Usage

To use the program, compile it and run the executable. You can then input commands and data to perform SkipList operations. Invalid commands will result in the "OPERACAO INVALIDA" message.

This program provides a versatile and interactive way to work with SkipLists, enabling insertion, updating, removal, searching, and visualization of the data structure's contents.

## Compilation

### Build System, Language, and Tooling

We used a robust Makefile to streamline the development process. You can find more about this Makefile at [github.com/bcmaia/MasterMaker](https://github.com/bcmaia/MasterMaker). The code was compiled and tested using `gcc` with the C99, C11, and C17 standards, working as expected for those specifications.

### Environment

We tested the program on a `Pop!_OS 22.04 LTS` x86 Linux system. Therefore, it should work on most modern Debian-based Linux distributions.

### Setup

To download the needed tools on Pop!_OS or most modern Debian-based Linux distributions:

Downloading the compiler tooling:

```bash
sudo apt update
sudo apt install -y build-essential
```

Make sure you have Git installed:

```bash
sudo apt install -y git
```

To clone the repository:

```bash
git clone https://github.com/bcmaia/skiplist
```

### Compilation

To compile the project, navigate to the project's directory and run:

```bash
make all
```

To run:

```bash
make run
```