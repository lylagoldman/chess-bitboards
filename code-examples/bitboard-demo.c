#include <stdio.h>
#include <stdint.h>

/* ---------- File masks (for wraparound-safe shifts) ---------- */
#define FILE_A 0x0101010101010101ULL
#define FILE_H 0x8080808080808080ULL
#define NOT_FILE_A (~FILE_A)
#define NOT_FILE_H (~FILE_H)

/* ---------- Helper: print a bitboard as an 8x8 board ---------- */
void print_board(const char *label, uint64_t board) {
    printf("%s\n", label);
    for (int rank = 7; rank >= 0; rank--) {
        printf("%d | ", rank + 1);
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if (board & (1ULL << square))
                printf("X  ");
            else
                printf(".  ");
        }
        printf("\n");
    }
    printf("    a  b  c  d  e  f  g  h\n\n");
}

/* ---------- Set a bit (place a piece) ---------- */
uint64_t set_bit(uint64_t board, int square) {
    return board | (1ULL << square);
}

/* ---------- Clear a bit (remove a piece) ---------- */
uint64_t clear_bit(uint64_t board, int square) {
    return board & ~(1ULL << square);
}

/* ---------- Move a piece (XOR trick, non-capture) ---------- */
uint64_t move_piece(uint64_t board, int from, int to) {
    uint64_t move_mask = (1ULL << from) | (1ULL << to);
    return board ^ move_mask;
}

/* ---------- Shift one square west, with wraparound protection ---------- */
uint64_t shift_west_safe(uint64_t board) {
    return (board & NOT_FILE_A) >> 1;
}

/* ---------- Shift one square east, with wraparound protection ---------- */
uint64_t shift_east_safe(uint64_t board) {
    return (board & NOT_FILE_H) << 1;
}

/* ---------- Shift one square west, WITHOUT protection (to show the bug) ---------- */
uint64_t shift_west_buggy(uint64_t board) {
    return board >> 1;
}

int main(void) {
    /* ===================================================== */
    /* 1. SET: place a white knight on b1 (bit 1)             */
    /* ===================================================== */
    uint64_t white_knights = 0;
    print_board("Empty board:", white_knights);

    white_knights = set_bit(white_knights, 1);  /* b1 */
    print_board("After setting b1 (white knight placed):", white_knights);

    /* ===================================================== */
    /* 2. SET another piece: knight on g1 (bit 6)             */
    /* ===================================================== */
    white_knights = set_bit(white_knights, 6);  /* g1 */
    print_board("After also setting g1 (second knight):", white_knights);

    /* ===================================================== */
    /* 3. CLEAR: remove the knight from b1                    */
    /* ===================================================== */
    white_knights = clear_bit(white_knights, 1); /* remove b1 */
    print_board("After clearing b1 (knight removed):", white_knights);

    /* ===================================================== */
    /* 4. MOVE: move the remaining knight from g1 to f3       */
    /*    g1 = bit 6, f3 = bit 21 (rank*8+file = 2*8+5)       */
    /* ===================================================== */
    int from = 6;   /* g1 */
    int to   = 21;  /* f3 */
    white_knights = move_piece(white_knights, from, to);
    print_board("After moving knight g1 -> f3:", white_knights);

    /* ===================================================== */
    /* 5. WRAPAROUND BUG: shift a piece on the a-file west    */
    /* ===================================================== */
    uint64_t edge_piece = set_bit(0, 24); /* a4 */
    print_board("Piece on a4, before any shift:", edge_piece);

    uint64_t buggy_result = shift_west_buggy(edge_piece);
    print_board("BUGGY shift west (board >> 1) -- wraps to h3!:", buggy_result);

    /* ===================================================== */
    /* 6. WRAPAROUND FIX: same shift, using FILE_A mask       */
    /* ===================================================== */
    uint64_t safe_result = shift_west_safe(edge_piece);
    print_board("SAFE shift west (masked with ~FILE_A) -- piece vanishes correctly:", safe_result);

    /* ===================================================== */
    /* 7. Shift a non-edge piece to show the mask doesn't     */
    /*    break normal shifts                                 */
    /* ===================================================== */
    uint64_t middle_piece = set_bit(0, 27); /* d4 */
    print_board("Piece on d4, before shift:", middle_piece);

    uint64_t middle_shifted = shift_west_safe(middle_piece);
    print_board("Safe shift west -- correctly lands on c4:", middle_shifted);

    /* ===================================================== */
    /* 8. CAPTURE simulation: white knight captures on c3     */
    /* ===================================================== */
    uint64_t white_knights2 = set_bit(0, 1);   /* knight on b1 */
    uint64_t black_knights  = set_bit(0, 18);  /* enemy knight on c3 */

    print_board("Before capture -- white knight (b1):", white_knights2);
    print_board("Before capture -- black knight (c3):", black_knights);

    int cap_from = 1;   /* b1 */
    int cap_to   = 18;  /* c3 */
    uint64_t move_mask    = (1ULL << cap_from) | (1ULL << cap_to);
    uint64_t capture_mask = (1ULL << cap_to);

    white_knights2 ^= move_mask;        /* mover: XOR toggles both bits   */
    black_knights  &= ~capture_mask;    /* captured piece: force-clear    */

    print_board("After capture -- white knight now on c3:", white_knights2);
    print_board("After capture -- black knight removed:", black_knights);

    return 0;
}