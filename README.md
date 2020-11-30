# x16-8sh
8Shell for the X16

# PURPOSE
A shell is a command line interpreter. It provides:
* common system commands and utilities, for example for files.  Pipes are useful system utilities.
* environment variables, which represent system status and user-defined state.
* a scripting language, for example to allow conditional command execution and state checking.

# The current binary:
* uses a repl.  the entered line is copied into Bank 1.
* specifically looks for "exit" to quit, otherwise:
* tokenizes the contents of Bank 1, storing type and pointers into Bank 2.
* compiles single expressions to bytecodes.
* executes bytecodes on its VM.
* is not especially optimized.

It has a Makefile and uses cc65.

It currently needs:
* a hashtable for environment variables.
* a heap manager (Alex Johnson has provided one).
* binop, assignment, interpolation, and function calls.

The code is based on Bob Nystrom's CRAFTING INTERPRETERS document.

# GOAL 1 ACHIEVED: REPL with prompt.
When run, 8sh will print a percentage sign and a space:
% 

# GOAL 2 ACHIEVED: EXIT command.
Type 'exit' to exit.

# GOAL 3 ACHIEVED: Source stored in Bank 1.
The input buffer is copied into Bank 1 before processing.
This decouples input from tokenization.

# GOAL 4 ACHIEVED: Token metadata written to Bank 2.
The tokenization pass completely tokenizes the input stream,
storing the metadata about each token in Bank 2.  Each token
metadata is 6 bytes long, and points back to the source text
in Bank 1 as needed.

# GOAL 5 ACHIEVED: Expression parsing.
Arithmetic expressions compile and run.  They can be stored
in a group (parentheses) as well, but only the first full 
expression is interpreted.
Binary is 15.5k.

# GOAL 6, ACHIEVED: multiple values and comparison operations.
Added true, false, and nil value types.
Added == >= <= < !
Binary is 16.8k.

# GOAL 7, ACHIEVED: strings and string ops added.
Binary is 18k.
