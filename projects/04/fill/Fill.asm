// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed.
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

(BEGIN)

   // Probe the keyboard
   @24576
   D=M
   @WHITE
   D;JEQ

   (BLACK)
      @val
      M=-1  // Fills pixels with black
      @FILL
      0;JMP

   (WHITE)
      @val
      M=0  // Fills pixels with white

   (FILL)

      @SCREEN
      D=A
      @idx
      M=D
      @8192 // # registers in the screen : 265 bits * 512 bits / 16 bits per register
      D=D+A
      @end
      M=D

      (LOOPFILL)

         // First, check if we've reached the end of the screen
         // Jump to the top of the program if so.
         @end
         D=M
         @idx
         D=D-M
         @BEGIN
         D;JLE

         // Fill this register with the value set by the keyboard press
         @val
         D=M
         @idx
         A=M
         M=D

         // Move on to the next register
         @idx
         M=M+1

         // Jump back to the beginning of the program if so
         @LOOPFILL
         0;JMP
