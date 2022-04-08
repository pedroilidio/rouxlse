# Roux LSE solver

Find a solution to the last six edges step (LSE) of roux method for solving
the Rubik's cube.

To specify a state of the cube, one must provide the current edge permuta-
tion, the edges' orientation and the positions of the middle and top layers.

For edges permutation, provide numbers 0 to 5 in the order they appear
in the cube, 0 to 5 representing the edges DF, DB, UB, UF, UL and UR, res-
pectively. So a sequence 425130 means UL is in DF's place, UB is in DB's
place, UR is in UB's place and so on.

For edge orientation, provide a 6-digit binary sequence, one digit for each
edge in the same order mentioned above, DF, DB, UB, UF, UL and UR. 0 means
the edge is oriented, 1 means it is misoriented. Note: the position of each
digit refer to the edge (ehich can be anywhere in the cube), NOT an edge
position. So 110000 means that the edges which should have been in the spots
DF and DB are misoriented.

The middle and upper layer positions are simply two integers from 0 to 3,
0 indicating that the layer is correctly aligned, 1 meaning the layer is a
single move (U or M, depending on the layer) apart from the solved position.
So an U move brings the top layer position to 1, another to 2, U' back to 1,
and so on, the same applying to the M layer.

Therefore, one should run the program with the arguments in the following
order:

```
$ ./roux_lse <edges position> <edges orientation> <M position> <U position>
```

## Run examples

```
$ ./roux_lse 012345 000011 2 2
     Target state: 012345 000011 2 2
     Visited 178936 states.
     Solution found!
     M U M' U M U' M U M2 U' M' U' M' U2

$ ./roux_lse 251304 100001 2 2
     Target state: 251304 100001 2 2
     Visited 157352 states.
     Solution found! 
     U2 M' U' M2 U2 M' U2 M' U M'

$ ./roux_lse 503124 000011 0 1
     Target state: 503124 000011 0 1
     Visited 103567 states.
     Solution found! 
     U' M U M U M' U' M U M2 U2 

$ ./roux_lse 130542 010111 1 3
    Target state: 130542 010111 1 3
    Visited 163445 states.
    Solution found! 
    M' U' M' U M' U M' U2 M U' M2 U'
```
