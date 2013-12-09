Project Name: Semantic Analysis

NOTICE: COPY FROM OTHERS WILL BOTH GET ZERO!


Project Specification: 
  Please See PROGRAM Section, Chapter Five, Modern Compiler Implementation in C.
  (Page 103 to 124 for English Version, Page Youknow for Chinese Version.:))

Due Date: December 8th, 2012

Submission: make a tallball which contains all necessary source files.

Note: 
Compiling Error means Zero Point. Standard Test Platform is Linux version 2.6.32-5-amd64 (Debian 2.6.32-31).
Make sure that your output is the same as reference output.

TA in Charge: Xiao Wei (Uninstall)  wei_xiao@fudan.edu.cn

TODO
implement env.h and env.c 

semant.h: 
void SEM_transProg(A_exp exp);

implement semant.c 
        The four recursive functions
        void  SEM_trnasProg(A_exp exp);

that type checks exp.
        Handle recursive and mutually recursive declarations.
        Check that a "break" is within a "for" or "while"
        Illegal cycles in type declaration  

implement a main() (in parse.c) that: 
        calls the parser
        calls the function SEM_transProg()

