@2
D=M
@1
A=A+D
D=M
@0
A=M
M=D
@0
M=M+1
@3
D=A
@1
A=A+D
D=A
@0
A=M
M=D
@0
M=M-1
@0
A=M
D=M
A=A+1
A=M
M=D
@0
D=A
@0
A=M
M=D
@0
M=M+1
@4
D=M
@0
A=A+D
D=A
@0
A=M
M=D
@0
M=M-1
@0
A=M
D=M
A=A+1
A=M
M=D
@1
D=A
@0
A=M
M=D
@0
M=M+1
@4
D=M
@1
A=A+D
D=A
@0
A=M
M=D
@0
M=M-1
@0
A=M
D=M
A=A+1
A=M
M=D
@2
D=M
@0
A=A+D
D=M
@0
A=M
M=D
@0
M=M+1
@2
D=A
@0
A=M
M=D
@0
M=M+1
@0
M=M-1
@0
A=M
D=M
@0
M=M-1
@0
A=M
M=M-D
@0
M=M+1
@2
D=M
@0
A=A+D
D=A
@0
A=M
M=D
@0
M=M-1
@0
A=M
D=M
A=A+1
A=M
M=D
(MAIN_LOOP_START)
@2
D=M
@0
A=A+D
D=M
@0
A=M
M=D
@0
M=M+1
@0
M=M-1
@0
A=M
D=M
@COMPUTE_ELEMENT
D;JNE
@END_PROGRAM
0;JMP
(COMPUTE_ELEMENT)
@4
D=M
@0
A=A+D
D=M
@0
A=M
M=D
@0
M=M+1
@4
D=M
@1
A=A+D
D=M
@0
A=M
M=D
@0
M=M+1
@0
M=M-1
@0
A=M
D=M
@0
M=M-1
@0
A=M
M=M+D
@0
M=M+1
@4
D=M
@2
A=A+D
D=A
@0
A=M
M=D
@0
M=M-1
@0
A=M
D=M
A=A+1
A=M
M=D
@3
D=A
@1
A=A+D
D=M
@0
A=M
M=D
@0
M=M+1
@1
D=A
@0
A=M
M=D
@0
M=M+1
@0
M=M-1
@0
A=M
D=M
@0
M=M-1
@0
A=M
M=M+D
@0
M=M+1
@3
D=A
@1
A=A+D
D=A
@0
A=M
M=D
@0
M=M-1
@0
A=M
D=M
A=A+1
A=M
M=D
@2
D=M
@0
A=A+D
D=M
@0
A=M
M=D
@0
M=M+1
@1
D=A
@0
A=M
M=D
@0
M=M+1
@0
M=M-1
@0
A=M
D=M
@0
M=M-1
@0
A=M
M=M-D
@0
M=M+1
@2
D=M
@0
A=A+D
D=A
@0
A=M
M=D
@0
M=M-1
@0
A=M
D=M
A=A+1
A=M
M=D
@MAIN_LOOP_START
0;JMP
(END_PROGRAM)
