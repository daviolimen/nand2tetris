// bootstrap code
@256
D=A
@SP
M=D
// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1
// eq command
@SP
AM=M-1
D=M
A=A-1
D=M-D
M=-1
@__COMPARISON_LABEL_0
D;JEQ
@SP
A=M-1
M=0
(__COMPARISON_LABEL_0)
// if-gotoskibidi
@SP
AM=M-1
D=M
@skibidi
D;JNE
// goto noskibidi
@noskibidi
0;JMP
// label skibidi
(skibidi)
// push constant 1
@1
D=A
@SP
A=M
M=D
@SP
M=M+1
// goto end
@end
0;JMP
// label noskibidi
(noskibidi)
// push constant 2
@2
D=A
@SP
A=M
M=D
@SP
M=M+1
// goto end
@end
0;JMP
// label end
(end)
// goto end
@end
0;JMP
