# README - SkipList Operations

This C program provides a command-line interface to perform various operations on a SkipList data structure.

## Overview
A SkipList is a data structure that allows efficient insertion, deletion, and searching of items. The program accepts commands from the user to perform these operations.

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
