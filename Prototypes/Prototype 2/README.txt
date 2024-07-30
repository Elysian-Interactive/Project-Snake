Fixing the movement issue:
-> The way movement is handled here is that every frame
-> the position of the previous block is assigned to the current block

Alternate way
-> A new head is added in the direction of the movement
-> then the tail is removed
-> and with each iteration the head keeps growing in the given direction
-> and we keep deleting the tail

-> In case it eats a food we grow the body and keep following the same mechanic