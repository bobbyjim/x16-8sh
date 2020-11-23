# x16-8sh
8Shell for the X16

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

# GOAL 6, in progress: multiple values.
Adding more value types (currently only knows integers).

