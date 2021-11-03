**VIRTUAL MACHINE**

**LC3**

Implemented LC3 virtual machine in `c`.
LC3 uses 16bit addressing.

1. 8 general registers
2. 1 pc counter
3. 1 condition flag
4. 16 opcodes


**LC3 instruction set**

    BR,     // branch 
    ADD,    // add  
    LD,     // load 
    ST,     // store 
    JSR,    // jump register 
    AND,    // bitwise and 
    LDR,    // load register 
    STR,    // store register 
    RTI,    // unused 
    NOT,    // bitwise not 
    LDI,    // load indirect 
    STI,    // store indirect 
    JMP,    // jump 
    RES,    // reserved (unused) 
    LEA,    // load effective address 
    TRAP    // execute trap 

You can learn more about instructions set [HEAR](https://justinmeiners.github.io/lc3-vm/supplies/lc3-isa.pdf)


**BUILDING AND RUNNING**

No dependencies

`VM` takes one source file as command line argument which contains LC3 instructions, vm will parse the asm file and create a array of
instructions and execute to produce resutls.

Program must start with .orig and end with .end directive.

example file:
```assembly
  .ORIG x3000
    ADD R0, R0, #1
    OUT
    HALT
  .END
 ```

Tested on `Linux Ubuntu` with `gcc c11`

First create executable using `make`


```assembly
make debug, make main
```

And run ``` ./main test1.asm```

Currently three tests are done, more tests needed.

And code naming and arrangements need improvements, will work on it.

Help always welcome.


