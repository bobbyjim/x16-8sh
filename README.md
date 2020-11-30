# x16-8sh
8Shell for the X16

# PURPOSE
A shell is a command line interpreter. It provides:
* common system commands and utilities, for example for files.  Pipes are useful system utilities.
* environment variables, which represent system status and user-defined state.
* a scripting language, for example to allow conditional command execution and state checking.

# Architecture
The Commander X16 has two RAM areas: a main program area nearly 40K in size, and a set of 8K "high RAM" 
banks for data storage.  I plan to make full use of them for storing everything I can there.  For now, 
I am using two banks: one for the script source (for now, that is the repl input line), and one to hold 
token metadata.

I think it might be cool to load several scripts at once.  Assuming a limit of 8K per script,
I could use bank N to store scripts, bank N+2 for its tokens, and bank N+3 for output.  Maybe.
We'll see.  If I did it this way, then piping data from script to script might be as simple 
as specifying bank numbers between pipes, e.g.

1 | 2 | 3.

I.E. run the script in location 1, pipe it through 2, pipe that through 3.  Output could be
automagically stored in Script 3's output bank.  Or something.

I could use a bank to store a tokenization-string datastore -- maybe I can put a tokenization 
"search tree" there to reduce the memory load of the main program.

Similarly, I might try storing the Pratt Parser data in banked RAM.  This could reduce the
main binary by 1K or more, although I would need code that accesses this data.  We'll see.

# The current binary:
* uses a repl.  the entered line is copied into Bank 1.
* specifically looks for "exit" to quit, otherwise:
* tokenizes the contents of Bank 1, storing type and pointers into Bank 2.
* executes bytecodes on its VM.
* + - / * integer math supported.
* boolean ops > >= == != <= < supported.
* strings and ops (eq ne gte lte gt lt) supported.

It has a Makefile and uses cc65.

The code is based on Bob Nystrom's CRAFTING INTERPRETERS document.

# Source stored in Bank 1.
The input buffer is copied into Bank 1 before processing.
This decouples input from tokenization.

# Token metadata written to Bank 2.
The tokenization pass completely tokenizes the input stream,
storing the metadata about each token in Bank 2.  Each token
metadata is 6 bytes long, and points back to the source text
in Bank 1 as needed.

# GOAL ACHIEVED: Expression parsing.
Arithmetic expressions compile and run.  They can be stored
in a group (parentheses) as well, but only the first full 
expression is interpreted.
Binary is 15.5k.

# GOAL ACHIEVED: multiple values and comparison operations.
Added true, false, and nil value types.
Added == >= <= < !
Binary is 16.8k.

# GOAL ACHIEVED: strings and string ops added.
Binary is 18k.
