// stack init
@256
D=A
@SP
M=D
// push constant 111
@111
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 333
@333
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 888
@888
D=A
@SP
A=M
M=D
@SP
M=M+1
// pop static 8
@teste.8
D=A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D
// pop static 3
@teste.3
D=A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D
// pop static 1
@teste.1
D=A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D
// push static 3
@teste.3
D=M
@SP
A=M
M=D
@SP
M=M+1
// push static 1
@teste.1
D=M
@SP
A=M
M=D
@SP
M=M+1
// sub command
@SP
AM=M-1
D=M
A=A-1
D=M-D
M=D
// push static 8
@teste.8
D=M
@SP
A=M
M=D
@SP
M=M+1
// add command
@SP
AM=M-1
D=M
A=A-1
D=D+M
M=D
