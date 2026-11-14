/*
 * bitboard_shifts.c
 *
 * Shows how shifting a 64-bit bitboard moves chess pieces around the board.
 *
 * Square numbering: a1 = 0, b1 = 1, ... h1 = 7, a2 = 8, ... h8 = 63
 *                   index = 8 * rank + file   (both counted from 0)
 *
 * Build and run:
 *     gcc -Wall -Wextra -std=c11 bitboard_shifts.c -o bitboard_shifts
 *     ./bitboard_shifts
 */

#include <stdio.h>
#include <stdint.h>

typedef uint64_t Bitboard;

/* ---- Useful constant masks ------------------------------------------- */

#define RANK_3      0x0000000000FF0000ULL   /* every square on rank 3       */
#define NOT_A_FILE  0xFEFEFEFEFEFEFEFEULL   /* every square EXCEPT the a-file */
#define NOT_H_FILE  0x7F7F7F7F7F7F7F7FULL   /* every square EXCEPT the h-file */

/* A bitboard with exactly one bit set: the square `sq`. Note the ULL! */
#define SQ(sq) (1ULL << (sq))

/* ---- The eight "one step" moves ---------------------------------------
 *
 * Moving one rank up (north) adds 8 to the square index  -> shift left 8.
 * Moving one file right (east) adds 1                    -> shift left 1.
 * Moving down or left subtracts                          -> shift right.
 *
 * North/south shifts need no mask: bits pushed past bit 63 (or below
 * bit 0) simply fall off the integer, which is exactly "off the board".
 *
 * Anything that moves east or west needs a mask, because the bits are
 * stored in one long line and would otherwise wrap onto the next rank.
 */
Bitboard north(Bitboard b)      { return  b << 8; }
Bitboard south(Bitboard b)      { return  b >> 8; }
Bitboard east(Bitboard b)       { return (b << 1) & NOT_A_FILE; }
Bitboard west(Bitboard b)       { return (b >> 1) & NOT_H_FILE; }
Bitboard north_east(Bitboard b) { return (b << 9) & NOT_A_FILE; }
Bitboard north_west(Bitboard b) { return (b << 7) & NOT_H_FILE; }
Bitboard south_east(Bitboard b) { return (b >> 7) & NOT_A_FILE; }
Bitboard south_west(Bitboard b) { return (b >> 9) & NOT_H_FILE; }

/* ---- Printing ---------------------------------------------------------
 *
 * Print rank 8 first so the output looks like a board seen from White's
 * side.  '1' = bit set (piece present), '.' = bit clear.
 */
void print_bb(const char *title, Bitboard b)
{
    printf("%s\n", title);
    for (int rank = 7; rank >= 0; rank--) {
        printf("  %d  ", rank + 1);
        for (int file = 0; file < 8; file++) {
            int sq = 8 * rank + file;
            putchar((b & SQ(sq)) ? '1' : '.');
            putchar(' ');
        }
        putchar('\n');
    }
    printf("     a b c d e f g h      0x%016llx\n\n", (unsigned long long)b);
}

int main(void)
{
    /* ------------------------------------------------------------------ */
    printf("=== 1. One piece, eight directions ===\n\n");
    Bitboard d4 = SQ(27);                       /* d4 = 8*3 + 3 = 27 */
    print_bb("Piece on d4 (square 27):", d4);
    print_bb("north(d4) = d4 << 8   -> d5 (35):", north(d4));
    print_bb("east(d4)  = d4 << 1   -> e4 (28):", east(d4));
    print_bb("All 8 neighbours OR'd together (a king's moves):",
             north(d4) | south(d4) | east(d4) | west(d4) |
             north_east(d4) | north_west(d4) |
             south_east(d4) | south_west(d4));

    /* ------------------------------------------------------------------ */
    printf("=== 2. Pawn pushes: moving ALL pawns at once ===\n\n");
    Bitboard white_pawns = 0x000000000000FF00ULL;           /* rank 2 */
    Bitboard blockers    = SQ(20) | SQ(25);                 /* e3, b4 */
    Bitboard occupied    = white_pawns | blockers;
    Bitboard empty       = ~occupied;

    print_bb("white_pawns:", white_pawns);
    print_bb("blockers (black pieces on e3 and b4):", blockers);
    print_bb("white_pawns << 8  (every pawn moved up one rank):",
             white_pawns << 8);

    Bitboard single_push = (white_pawns << 8) & empty;
    print_bb("single_push = (white_pawns << 8) & empty   (e3 is blocked):",
             single_push);

    Bitboard double_push = ((single_push & RANK_3) << 8) & empty;
    print_bb("double_push = ((single_push & RANK_3) << 8) & empty\n"
             "  (e-pawn never reached rank 3; b4 is blocked):",
             double_push);

    /* ------------------------------------------------------------------ */
    printf("=== 3. The wraparound problem ===\n\n");
    Bitboard h4 = SQ(31);                       /* h4 = 8*3 + 7 = 31 */
    print_bb("Piece on h4 (square 31):", h4);
    print_bb("h4 << 1  WITHOUT a mask -> wrongly lands on a5 (32):", h4 << 1);
    print_bb("NOT_A_FILE mask:", NOT_A_FILE);
    print_bb("(h4 << 1) & NOT_A_FILE -> empty board (correct: no square east of h):",
             east(h4));

    return 0;
}
