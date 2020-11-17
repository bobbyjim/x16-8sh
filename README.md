# x16-8sh
8Shell for the X16

# GOAL 1 ACHIEVED: REPL with prompt.
When run, 8sh will print a percentage sign and a space:
% 

# GOAL 2 ACHIEVED: EXIT command.
Type 'exit' to exit.

# GOAL 3 ACHIEVED: REG command.
Registers A-D are mapped to r1-r4 in the X16.
Type 'reg' to see registers A-D (r1-r4)

# GOAL 4: TIME command.
Type "time" to get system time called and printed.



# GOAL N: To be able to treat the input stream AS the token stream TOO.
To do this, I'd need enough room on all tokens to store the token type
and its length at least.

This means I'd overwrite the first char of each token with its type
(0-255), and null-terminate the string.

I'd be clever so that TOKEN_VAR is 36 ('$').  I suppose I'd have to 
allocate ten tokens for numbers (48-57, '0'-'9').


