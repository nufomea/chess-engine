#pragma once

#include <cstdint>
#include <string>

typedef uint64_t U64;
typedef uint8_t U8;

static U64 bishop_rays[5248];
static U64 rook_rays[102400];
static U64 between[64][64];
static U64 board_hash[12][64];

enum Color : bool {
	WHITE = true,
	BLACK = false
};

enum Square : U8 {
	A8, B8, C8, D8, E8, F8, G8, H8,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A1, B1, C1, D1, E1, F1, G1, H1
};

enum PieceIndex : U8 {
	WHITE_PAWN,
	WHITE_KNIGHT,
	WHITE_BISHOP,
	WHITE_ROOK,
	WHITE_QUEEN,
	WHITE_KING,
	BLACK_PAWN,
	BLACK_KNIGHT,
	BLACK_BISHOP,
	BLACK_ROOK,
	BLACK_QUEEN,
	BLACK_KING,
	EMPTY
};

struct MagicEntry {
	U64 mask;
	U64 magic;
	U8 shift;
	int offset;
};

struct Move {
	U8 from;
	U8 to;

	U8 promotion;
};

struct BoardState {
	U64 bitboards[12];

	// 캐슬링 권한
	bool WKC, WQC, BKC, BQC;
	// 움직일 차례
	bool turn;

	// 앙파상 대상
	U64 enpassant;
	// 50수 규칙
	U8 half_moves;
	// zobrist 해쉬
	U64 hash;

	U64 occupied_white;
	U64 occupied_black;
};

class Board {
private:
	BoardState* state_stack;
	int stack_index;

	U64 white_pawn_attacks[64];
	U64 black_pawn_attacks[64];
	U64 knight_attacks[64];
	U64 bishop_attacks[64];
	U64 rook_attacks[64];
	U64 king_attacks[64];

	U64 enpassant_hash[8];
	U64 turn_hash;
	U64 WKC_hash;
	U64 WQC_hash;
	U64 BKC_hash;
	U64 BQC_hash;

public:
	BoardState state;

	U64 occupied_all;

	std::string draw_ascii_board();

	void set_piece_at(U8 piece, U8 square);

	template <bool color>
	void push_impl(const Move& move);
	void push(const Move& move);
	void pop();

	template <bool turn>
	int get_legal_moves_impl(Move* moves);
	int get_legal_moves(Move* moves);

	template <bool turn>
	bool is_attacked(U8 square, U64 occupied);
	bool is_checked();

	void set_position(std::string fen);

	U64 generate_attacks(U8 square,	U64 occupied, const int dir_file[],	const int dir_rank[], int dir_count, int max_depth);

	Board();
};
