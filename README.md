# ♛♔ Chess Bitboards (Checkoff 3)
I chose to learn about a real-world application of bitwise operators: **chess bitboards**. I explored how chess programs store piece types, piece colors, and occupied spaces in 64-bit unsigned integers. Additionally, I learned more about how chess programs use bitwise operations to manipulate, represent, and update the state of the chessboard.

### Most Interesting Discoveries
**The Wraparound Problem**
- Since chess programs use `uint64_t` variables to represent the board instead of a 2D array, the bitwise operations alone to shift left/right by 1 do not account for the "edges" of the board.
- A piece sitting on file a (leftmost column) or file h (rightmost column) shifted one more square in that direction would end up one rank (row) away, on the opposite end of the board.
- Solution: `#define FILE_A` and `#define FILE_H`, then before each sideways shift, `AND` the board with `~FILE_A` or `~FILE_H`, then shift by 1.
- Result: A shift that would normally result in the piece being "wrapped around" the board is simply removed from the board.

**Symmetric Attack Logic**
- To validate if a king is in check, chess programs DO NOT loop through all enemy pieces and checking if they can attack the king.
- Instead, the opposite logic is used-- if the king is a different type of piece, can it attack any of that type on the board?
- E.g. if there is a rook in the king's square, if it can attack any enemy rook on the board, then the king is in check.
- This concept uses precomputed attack tables for each piece type → much faster than looping through all pieces!
