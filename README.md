# CS220
Coursework from CS220 Computer Organization

Program 1:

This program asks the user for number and a base, then converts the number to decimal and binary and reports it to the user.


Program 2:

This program uses a lexical analyzer supplied by the instructor as a base.  The base was adapted to take the input of a file such as "p2in.txt" and find bit strings of at most 8 bits.

The program then sign extends the bitstrings to be 8-bits in length, makes the value unsigned and prints out the result.  Makes the value signed and prints the result.  Makes the value it's 1s compliment and prints it out.  Then makes the value it's 2s compliment and prints it out.

It does this for all bit strings until it gets to the end of the file.


Program 3:

This program uses a fixed point library supplied by the instructor as a base.  The program then goes through the process of being an automated cash register when you're buying bananas, using only integer and fixed point numbers.




ATRobots Programs:

These programs are robots used by the robot fighting simulator ATRobots which you can find here https://drive.google.com/file/d/0B_QUjyn5aOC3dElpMzlqSlI1cDA/view?usp=sharing
They are written in a assembly-like language specifically designed for ATRobots which loosely resembles the Intel assembly language.


ECKSCAN:

This robot sits in one spot, uses a binary search to scan for the enemy. Once the robot believes it is accurate enough, it shoots at them.


ECKSERP:

This robot moves in a serpentine-like pattern around the arena by either moving in straight line, quater, half, or three quarter circles.  If it hits a wall it turns around and drives away from the wall.


GRID:

This robot divides the whole arena into a grid, then proceeds to travel a specified pattern through the arena.  At specific points it lays mines, then when it gets to the center it detonates them.

NAWDDAWG:

This robot combines ECKSCAN and ECKSERP, it travels around the arena in a serpentine pattern while doing a binary search to scan for nearby robots, when it's scans become accurate enough it shoots at them.
