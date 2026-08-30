#include "board.h"
#include "bitops.h"
#include <iostream>
#include <random>
#include <sstream>

using namespace std;

constexpr MagicEntry ROOK_MAGICS[64] = {
	{ 0x000101010101017e, 0x0480002810804000, 52, 0 },
	{ 0x000202020202027c, 0x0240004020001008, 53, 4096 },
	{ 0x000404040404047a, 0x0080200080100008, 53, 6144 },
	{ 0x0008080808080876, 0x1280040800809000, 53, 8192 },
	{ 0x001010101010106e, 0x0200020010042008, 53, 10240 },
	{ 0x002020202020205e, 0x6200020001841008, 53, 12288 },
	{ 0x004040404040403e, 0x0200240081020048, 53, 14336 },
	{ 0x008080808080807e, 0x8a00008100403402, 52, 16384 },
	{ 0x0001010101017e00, 0x0248800080400029, 53, 20480 },
	{ 0x0002020202027c00, 0x0080402010004000, 54, 22528 },
	{ 0x0004040404047a00, 0x0508802002801000, 54, 23552 },
	{ 0x0008080808087600, 0x4241000810010020, 54, 24576 },
	{ 0x0010101010106e00, 0x0110800400810802, 54, 25600 },
	{ 0x0020202020205e00, 0x1031002e04005900, 54, 26624 },
	{ 0x0040404040403e00, 0x000400010c023098, 54, 27648 },
	{ 0x0080808080807e00, 0x0801000082004100, 53, 28672 },
	{ 0x00010101017e0100, 0x1000928000400020, 53, 30720 },
	{ 0x00020202027c0200, 0x009000c020004004, 54, 32768 },
	{ 0x00040404047a0400, 0x0500808020001001, 54, 33792 },
	{ 0x0008080808760800, 0x0000818048001000, 54, 34816 },
	{ 0x00101010106e1000, 0x0880050010080100, 54, 35840 },
	{ 0x00202020205e2000, 0x0400808002000400, 54, 36864 },
	{ 0x00404040403e4000, 0x9090040010020108, 54, 37888 },
	{ 0x00808080807e8000, 0x001002000840810c, 53, 38912 },
	{ 0x000101017e010100, 0x0000400480048220, 53, 40960 },
	{ 0x000202027c020200, 0x0200400040201002, 54, 43008 },
	{ 0x000404047a040400, 0x0000402200108200, 54, 44032 },
	{ 0x0008080876080800, 0x101d050900100020, 54, 45056 },
	{ 0x001010106e101000, 0x1200050100080010, 54, 46080 },
	{ 0x002020205e202000, 0x1002020080040080, 54, 47104 },
	{ 0x004040403e404000, 0x0118ee0400100118, 54, 48128 },
	{ 0x008080807e808000, 0x222804020000a041, 53, 49152 },
	{ 0x0001017e01010100, 0x2480004000402000, 53, 51200 },
	{ 0x0002027c02020200, 0x0580400080802008, 54, 53248 },
	{ 0x0004047a04040400, 0x0000200080801004, 54, 54272 },
	{ 0x0008087608080800, 0x0001002409001000, 54, 55296 },
	{ 0x0010106e10101000, 0x2001800801801400, 54, 56320 },
	{ 0x0020205e20202000, 0x1800800200800400, 54, 57344 },
	{ 0x0040403e40404000, 0x1004102204000188, 54, 58368 },
	{ 0x0080807e80808000, 0x021006831200004c, 53, 59392 },
	{ 0x00017e0101010100, 0x8020408001010020, 53, 61440 },
	{ 0x00027c0202020200, 0x0040080030012000, 54, 63488 },
	{ 0x00047a0404040400, 0x0060004021010010, 54, 64512 },
	{ 0x0008760808080800, 0x5000200812020040, 54, 65536 },
	{ 0x00106e1010101000, 0x0001004800050030, 54, 66560 },
	{ 0x00205e2020202000, 0x0000040002008080, 54, 67584 },
	{ 0x00403e4040404000, 0x0010085026140005, 54, 68608 },
	{ 0x00807e8080808000, 0x0010048404460003, 53, 69632 },
	{ 0x007e010101010100, 0x400d034021800100, 53, 71680 },
	{ 0x007c020202020200, 0x0000201080400c80, 54, 73728 },
	{ 0x007a040404040400, 0x0000142001024500, 54, 74752 },
	{ 0x0076080808080800, 0x1040092202401200, 54, 75776 },
	{ 0x006e101010101000, 0x001a080004008080, 54, 76800 },
	{ 0x005e202020202000, 0x8094010040020040, 54, 77824 },
	{ 0x003e404040404000, 0x1008701108020400, 54, 78848 },
	{ 0x007e808080808000, 0x4a0000440100a200, 53, 79872 },
	{ 0x7e01010101010100, 0x1441048000102041, 52, 81920 },
	{ 0x7c02020202020200, 0x4000881040010021, 53, 86016 },
	{ 0x7a04040404040400, 0x0080401104082001, 53, 88064 },
	{ 0x7608080808080800, 0x8010080500201001, 53, 90112 },
	{ 0x6e10101010101000, 0x2881001004080023, 53, 92160 },
	{ 0x5e20202020202000, 0x0021000400080203, 53, 94208 },
	{ 0x3e40404040404000, 0x8000020100881004, 53, 96256 },
	{ 0x7e80808080808000, 0x0431010400204082, 52, 98304 },
};

constexpr MagicEntry BISHOP_MAGICS[64]{
	{ 0x0040201008040200, 0x0050341820440820, 58, 0 },
	{ 0x0000402010080400, 0x8045214204010000, 59, 64 },
	{ 0x0000004020100a00, 0x0004044412400602, 59, 96 },
	{ 0x0000000040221400, 0x8048060442001000, 59, 128 },
	{ 0x0000000002442800, 0x040202100000000a, 59, 160 },
	{ 0x0000000204085000, 0x2881012940001003, 59, 192 },
	{ 0x0000020408102000, 0x00810858044400a0, 59, 224 },
	{ 0x0002040810204000, 0x8022840042022040, 58, 256 },
	{ 0x0020100804020000, 0x40d040090e040446, 59, 320 },
	{ 0x0040201008040000, 0x400490011101020a, 59, 352 },
	{ 0x00004020100a0000, 0x2921082204002020, 59, 384 },
	{ 0x0000004022140000, 0x0450040420801201, 59, 416 },
	{ 0x0000000244280000, 0x8008071040002800, 59, 448 },
	{ 0x0000020408500000, 0x03000e024221b008, 59, 480 },
	{ 0x0002040810200000, 0x0103221824020880, 59, 512 },
	{ 0x0004081020400000, 0x0090009400821011, 59, 544 },
	{ 0x0010080402000200, 0x0041202019010510, 59, 576 },
	{ 0x0020100804000400, 0x0020400282040100, 59, 608 },
	{ 0x004020100a000a00, 0x0204103202220200, 57, 640 },
	{ 0x0000402214001400, 0x800402280a102069, 57, 768 },
	{ 0x0000024428002800, 0x920c0002021508a0, 57, 896 },
	{ 0x0002040850005000, 0x000a000103050150, 57, 1024 },
	{ 0x0004081020002000, 0x0002010082212004, 59, 1152 },
	{ 0x0008102040004000, 0x00a4200a06220240, 59, 1184 },
	{ 0x0008040200020400, 0x8402082010101018, 59, 1216 },
	{ 0x0010080400040800, 0x8008600004044088, 59, 1248 },
	{ 0x0020100a000a1000, 0x2000881010002021, 57, 1280 },
	{ 0x0040221400142200, 0x0082080054004108, 55, 1408 },
	{ 0x0002442800284400, 0x0001010018104011, 55, 1920 },
	{ 0x0004085000500800, 0x0002002184100800, 57, 2432 },
	{ 0x0008102000201000, 0x00020a2446880100, 59, 2560 },
	{ 0x0010204000402000, 0x0006004000884801, 59, 2592 },
	{ 0x0004020002040800, 0x1208201005050400, 59, 2624 },
	{ 0x0008040004081000, 0x00280802b0041400, 59, 2656 },
	{ 0x00100a000a102000, 0x0808211010010401, 57, 2688 },
	{ 0x0022140014224000, 0x8000608020080200, 55, 2816 },
	{ 0x0044280028440200, 0x01840040100c0101, 55, 3328 },
	{ 0x0008500050080400, 0x2002082200104050, 57, 3840 },
	{ 0x0010200020100800, 0x0008820080840094, 59, 3968 },
	{ 0x0020400040201000, 0x0084010043821042, 59, 4000 },
	{ 0x0002000204081000, 0x0004109804840848, 59, 4032 },
	{ 0x0004000408102000, 0x0021017002041000, 59, 4064 },
	{ 0x000a000a10204000, 0x1800140208000101, 57, 4096 },
	{ 0x0014001422400000, 0x080c684012043040, 57, 4224 },
	{ 0x0028002844020000, 0x0000082304000640, 57, 4352 },
	{ 0x0050005008040200, 0x0002040800220200, 57, 4480 },
	{ 0x0020002010080400, 0x4002440444100580, 59, 4608 },
	{ 0x0040004020100800, 0x0001120202102841, 59, 4640 },
	{ 0x0000020408102000, 0xb822009008888080, 59, 4672 },
	{ 0x0000040810204000, 0x04d6008c21880010, 59, 4704 },
	{ 0x00000a1020400000, 0x008201a108481040, 59, 4736 },
	{ 0x0000142240000000, 0x2100080041108000, 59, 4768 },
	{ 0x0000284402000000, 0x1200004010248114, 59, 4800 },
	{ 0x0000500804020000, 0x0004040448020008, 59, 4832 },
	{ 0x0000201008040200, 0x0084200424008202, 59, 4864 },
	{ 0x0000402010080400, 0x0044442082060821, 59, 4896 },
	{ 0x0002040810204000, 0x0003028050088400, 58, 4928 },
	{ 0x0004081020400000, 0x2082320104111500, 59, 4992 },
	{ 0x000a102040000000, 0x0040180080480801, 59, 5024 },
	{ 0x0014224000000000, 0x00034080a020a808, 59, 5056 },
	{ 0x0028440200000000, 0x012901c020042400, 59, 5088 },
	{ 0x0050080402000000, 0x6000001003100100, 59, 5120 },
	{ 0x0020100804020000, 0x0000222410020049, 59, 5152 },
	{ 0x0040201008040200, 0x0008481004004011, 58, 5184 }
};

string Board::draw_ascii_board() {
	string ascii_board = "";

	for (int i = 0; i < 64; i++) {
		U64 mask = U64(1) << i;
		int piece = EMPTY;
		for (int i = 0; i < 12; i++) {
			if (state.bitboards[i] & mask) {
				piece = i;
				break;
			}
		}

		if (piece == WHITE_PAWN) ascii_board += "P";
		else if (piece == WHITE_KNIGHT) ascii_board += "N";
		else if (piece == WHITE_BISHOP) ascii_board += "B";
		else if (piece == WHITE_ROOK) ascii_board += "R";
		else if (piece == WHITE_QUEEN) ascii_board += "Q";
		else if (piece == WHITE_KING) ascii_board += "K";
		else if (piece == BLACK_PAWN) ascii_board += "p";
		else if (piece == BLACK_KNIGHT) ascii_board += "n";
		else if (piece == BLACK_BISHOP) ascii_board += "b";
		else if (piece == BLACK_ROOK) ascii_board += "r";
		else if (piece == BLACK_QUEEN) ascii_board += "q";
		else if (piece == BLACK_KING) ascii_board += "k";
		else ascii_board += ".";

		if ((i + 1) % 8 == 0) ascii_board += "\n";
	}

	return ascii_board;
}

void Board::set_piece_at(U8 piece, U8 square) {
	U64 mask = 1ULL << square;
	for (int i = 0; i < 12; i++) {
		if (i == piece) state.bitboards[i] |= mask;
		else state.bitboards[i] &= ~mask;
	}
}

template <bool turn>
inline void Board::push_impl(const Move &move) {
	constexpr int PAWN_US = (turn == WHITE) ? WHITE_PAWN : BLACK_PAWN;
	constexpr int PAWN_THEM = (turn == WHITE) ? BLACK_PAWN : WHITE_PAWN;
	constexpr int KING_US = (turn == WHITE) ? WHITE_KING : BLACK_KING;
	constexpr int ROOK_US = (turn == WHITE) ? WHITE_ROOK : BLACK_ROOK;

	constexpr int US_START = (turn == WHITE) ? 0 : 6;
	constexpr int US_END = (turn == WHITE) ? 6 : 12;
	constexpr int THEM_START = (turn == WHITE) ? 6 : 0;
	constexpr int THEM_END = (turn == WHITE) ? 11 : 5;

	state_stack[stack_index++] = state;
	state.half_moves++;

	U64 from_bb = U64(1) << move.from;
	U64 to_bb = U64(1) << move.to;
	U64 move_bb = from_bb | to_bb;
	U8 moved_piece = PAWN_US;

	// 기물 이동 처리
	for (int i = US_START; i < US_END; i++) {
		if (state.bitboards[i] & from_bb) {
			moved_piece = i;
			state.bitboards[i] ^= move_bb;

			state.hash ^= board_hash[moved_piece][move.from];
			state.hash ^= board_hash[moved_piece][move.to];

			break;
		}
	}
	if constexpr (turn == WHITE) {
		state.occupied_white ^= move_bb;
	}
	else {
		state.occupied_black ^= move_bb;
	}

	// 잡은 기물 처리
	for (int i = THEM_START; i < THEM_END; i++) {
		if (state.bitboards[i] & to_bb) {
			state.bitboards[i] ^= to_bb;

			// 기물 잡아서 50수 규칙 초기화
			state.half_moves = 0;

			if constexpr (turn == WHITE) {
				state.occupied_black ^= to_bb;
			}
			else {
				state.occupied_white ^= to_bb;
			}

			state.hash ^= board_hash[i][move.to];

			break;
		}
	}

	if (moved_piece == PAWN_US) {
		// 폰 움직여서 50수 규칙 초기화
		state.half_moves = 0;

		// 앙파상 처리
		if (to_bb == state.enpassant) {
			if constexpr (turn == WHITE) {
				U64 captured_bb = state.enpassant << 8;
				state.bitboards[BLACK_PAWN] ^= captured_bb;

				state.occupied_black ^= captured_bb;

				state.hash ^= board_hash[BLACK_PAWN][lsb_index(captured_bb)];
			}
			else {
				U64 captured_bb = state.enpassant >> 8;
				state.bitboards[WHITE_PAWN] ^= captured_bb;

				state.occupied_white ^= captured_bb;

				state.hash ^= board_hash[WHITE_PAWN][lsb_index(captured_bb)];
			}
		}

		if (state.enpassant) {
			// 앙파상 대상 업데이트
			state.hash ^= enpassant_hash[lsb_index(state.enpassant) % 8];
			state.enpassant = 0;
		}

		// 앙파상 가능할 경우 표시
		if (abs(move.from - move.to) == 16) {
			if constexpr (turn == WHITE) {
				U8 target = move.to + 8;
				if (state.bitboards[BLACK_PAWN] & white_pawn_attacks[target]) {
					state.enpassant = 1ULL << target;

					state.hash ^= enpassant_hash[target % 8];
				}
			}
			else {
				U8 target = move.to - 8;
				if (state.bitboards[WHITE_PAWN] & black_pawn_attacks[target]) {
					state.enpassant = 1ULL << target;

					state.hash ^= enpassant_hash[target % 8];
				}
			}
		}
	}
	// 앙파상 대상 업데이트
	else if (state.enpassant) {
		state.hash ^= enpassant_hash[lsb_index(state.enpassant) % 8];
		state.enpassant = 0;
	}

	// 캐슬링 권한 업데이트
	if (state.WKC && !(state.bitboards[WHITE_ROOK] & (1ULL << H1))) {
		state.WKC = false; state.hash ^= WKC_hash;
	}
	if (state.WQC && !(state.bitboards[WHITE_ROOK] & (1ULL << A1))) {
		state.WQC = false; state.hash ^= WQC_hash;
	}
	if (state.BKC && !(state.bitboards[BLACK_ROOK] & (1ULL << H8))) {
		state.BKC = false; state.hash ^= BKC_hash;
	}
	if (state.BQC && !(state.bitboards[BLACK_ROOK] & (1ULL << A8))) {
		state.BQC = false; state.hash ^= BQC_hash;
	}

	// 캐슬링 처리
	if (moved_piece == KING_US) {
		if constexpr (turn == WHITE) {
			if (move_bb == 0x5000000000000000) {
				state.bitboards[WHITE_ROOK] ^= 0xa000000000000000;

				state.occupied_white ^= 0xa000000000000000;

				state.hash ^= board_hash[WHITE_ROOK][H1];
				state.hash ^= board_hash[WHITE_ROOK][F1];
			}
			else if (move_bb == 0x1400000000000000) {
				state.bitboards[WHITE_ROOK] ^= 0x0900000000000000;

				state.occupied_white ^= 0x0900000000000000;

				state.hash ^= board_hash[WHITE_ROOK][A1];
				state.hash ^= board_hash[WHITE_ROOK][D1];
			}

			if (state.WKC) { state.WKC = false; state.hash ^= WKC_hash; }
			if (state.WQC) { state.WQC = false; state.hash ^= WQC_hash; }
		}
		else {
			if (move_bb == 0x0000000000000050) {
				state.bitboards[BLACK_ROOK] ^= 0x00000000000000a0;

				state.occupied_black ^= 0x00000000000000a0;

				state.hash ^= board_hash[BLACK_ROOK][H8];
				state.hash ^= board_hash[BLACK_ROOK][F8];
			}
			else if (move_bb == 0x0000000000000014) {
				state.bitboards[BLACK_ROOK] ^= 0x0000000000000009;

				state.occupied_black ^= 0x0000000000000009;

				state.hash ^= board_hash[BLACK_ROOK][A8];
				state.hash ^= board_hash[BLACK_ROOK][D8];
			}

			if (state.BKC) { state.BKC = false; state.hash ^= BKC_hash; }
			if (state.BQC) { state.BQC = false; state.hash ^= BQC_hash; }
		}
	}

	// 프로모션 처리
	if (move.promotion != EMPTY) {
		state.bitboards[moved_piece] ^= to_bb;
		state.bitboards[move.promotion] ^= to_bb;

		state.hash ^= board_hash[moved_piece][move.to];
		state.hash ^= board_hash[move.promotion][move.to];
	}

	state.turn = !turn;
	state.hash ^= turn_hash;
}

void Board::push(const Move &move) {
	if (state.turn == WHITE) {
		push_impl<WHITE>(move);
	}
	else {
		push_impl<BLACK>(move);
	}
}

void Board::pop() {
	if (stack_index > 0) {
		state = state_stack[--stack_index];
	}
}

template<bool turn>
inline int Board::get_legal_moves_impl(Move *moves) {
	constexpr U8 PAWN_US = (turn == WHITE) ? WHITE_PAWN : BLACK_PAWN;
	constexpr U8 KNIGHT_US = (turn == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT;
	constexpr U8 BISHOP_US = (turn == WHITE) ? WHITE_BISHOP : BLACK_BISHOP;
	constexpr U8 ROOK_US = (turn == WHITE) ? WHITE_ROOK : BLACK_ROOK;
	constexpr U8 QUEEN_US = (turn == WHITE) ? WHITE_QUEEN : BLACK_QUEEN;
	constexpr U8 KING_US = (turn == WHITE) ? WHITE_KING : BLACK_KING;
	constexpr U8 PAWN_THEM = (turn == WHITE) ? BLACK_PAWN : WHITE_PAWN;
	constexpr U8 KNIGHT_THEM = (turn == WHITE) ? BLACK_KNIGHT : WHITE_KNIGHT;
	constexpr U8 BISHOP_THEM = (turn == WHITE) ? BLACK_BISHOP : WHITE_BISHOP;
	constexpr U8 ROOK_THEM = (turn == WHITE) ? BLACK_ROOK : WHITE_ROOK;
	constexpr U8 QUEEN_THEM = (turn == WHITE) ? BLACK_QUEEN : WHITE_QUEEN;

	U64 occupied_us, occupied_them;
	if constexpr (turn == WHITE) {
		occupied_us = state.occupied_white;
		occupied_them = state.occupied_black;
	}
	else {
		occupied_us = state.occupied_black;
		occupied_them = state.occupied_white;
	}

	int num_moves = 0;
	int checked = 0;

	occupied_all = state.occupied_white | state.occupied_black;

	U64 bb_pawn = state.bitboards[PAWN_US];
	U64 bb_knight = state.bitboards[KNIGHT_US];
	U64 bb_bishop = state.bitboards[BISHOP_US] | state.bitboards[QUEEN_US];
	U64 bb_rook = state.bitboards[ROOK_US] | state.bitboards[QUEEN_US];

	U8 square;
	U8 king_square = lsb_index(state.bitboards[KING_US]);
	U64 x_ray;
	U64 check_mask = 0xFFFFFFFFFFFFFFFF;
	U64 pin_mask[64];

	memset(pin_mask, 0xFF, sizeof(pin_mask));

	U64 mask = knight_attacks[king_square] & state.bitboards[KNIGHT_THEM];
	if constexpr (turn == WHITE) {
		mask |= white_pawn_attacks[king_square] & state.bitboards[BLACK_PAWN];
	}
	else {
		mask |= black_pawn_attacks[king_square] & state.bitboards[WHITE_PAWN];
	}
	if (mask) {
		checked++;
		check_mask = mask;
	}

	x_ray = bishop_attacks[king_square] & (state.bitboards[BISHOP_THEM] | state.bitboards[QUEEN_THEM]);
	while (x_ray) {
		U8 attacker_square = pop_lsb(x_ray);
		U64 ray = between[king_square][attacker_square];
		U64 blockers = ray & occupied_all;
		int count = (int)__popcnt64(blockers);

		// ray가 언제나 공격자(to) 위치를 포함한다.
		// 따라서 count는 언제나 1보다 크다.
		if (count == 1) {
			checked++;

			check_mask = ray;
		}
		else if (count == 2 && (blockers & occupied_us)) {
			U64 target = blockers & occupied_us;
			if (target & state.bitboards[KNIGHT_US]) {
				bb_knight ^= target;
			}
			else if (target & state.bitboards[ROOK_US]) {
				bb_rook ^= target;
			}
			else {
				pin_mask[lsb_index(target)] = ray;
			}
		}
	}

	x_ray = rook_attacks[king_square] & (state.bitboards[ROOK_THEM] | state.bitboards[QUEEN_THEM]);
	while (x_ray) {
		U8 attacker_square = pop_lsb(x_ray);
		U64 ray = between[king_square][attacker_square];
		U64 blockers = ray & occupied_all;
		int count = (int)__popcnt64(blockers);

		// ray가 언제나 공격자(to) 위치를 포함한다.
		// 따라서 count는 언제나 1보다 크다.
		if (count == 1) {
			checked++;

			check_mask = ray;
		}
		else if (count == 2 && (blockers & occupied_us)) {
			U64 target = blockers & occupied_us;
			if (target & state.bitboards[KNIGHT_US]) {
				bb_knight ^= target;
			}
			else if (target & state.bitboards[BISHOP_US]) {
				bb_bishop ^= target;
			}
			else {
				pin_mask[lsb_index(target)] = ray;
			}
		}
	}

	// 킹 공격
	square = lsb_index(state.bitboards[KING_US]);
	U64 bb = king_attacks[square] & ~occupied_us;
	while (bb)
	{
		U8 target = pop_lsb(bb);
		U64 mask = occupied_all ^ ((1ULL << square) | (1ULL << target));
		if (!is_attacked<turn>(target, mask)) {
			moves[num_moves++] = { square, target, EMPTY };
		}
	}

	if (checked == 2) {
		return num_moves;
	}

	if constexpr (turn == WHITE) {
		// 폰 이동
		U64 bb = (bb_pawn >> 8) & ~occupied_all;
		U64 doubles = ((bb & 0x0000ff0000000000) >> 8) & ~occupied_all & check_mask;
		bb &= check_mask;
		// 일반 이동
		while (bb) {
			square = pop_lsb(bb);
			U8 origin = square + 8;

			if (!((1ULL << square) & pin_mask[origin])) continue;

			// 프로모션
			if (square < 8) {
				moves[num_moves++] = { origin, square, WHITE_QUEEN };
				moves[num_moves++] = { origin, square, WHITE_ROOK };
				moves[num_moves++] = { origin, square, WHITE_BISHOP };
				moves[num_moves++] = { origin, square, WHITE_KNIGHT };
			}
			else moves[num_moves++] = { origin, square, EMPTY };
		}
		// 2칸 이동
		while (doubles) {
			square = pop_lsb(doubles);
			U8 origin = square + 16;

			if (!((1ULL << square) & pin_mask[origin])) continue;

			moves[num_moves++] = { origin, square, EMPTY };
		}
		// 폰 공격
		bb = ((bb_pawn & 0xfefefefefefefefe) >> 9) & (state.occupied_black | state.enpassant) & check_mask;
		while (bb) {
			square = pop_lsb(bb);
			U8 origin = square + 9;

			if (!((1ULL << square) & pin_mask[origin])) continue;

			if (square < 8)
			{
				moves[num_moves++] = { origin, square, WHITE_QUEEN };
				moves[num_moves++] = { origin, square, WHITE_ROOK };
				moves[num_moves++] = { origin, square, WHITE_BISHOP };
				moves[num_moves++] = { origin, square, WHITE_KNIGHT };
			}
			else {
				if (square == state.enpassant) {
					// 앙파상 핀 검사
					U64 from = 1ULL << origin;
					U64 to = 1ULL << square;
					U64 captured = 1ULL << (square + 8);
					if (is_attacked<turn>(king_square, occupied_all ^ (from | to | captured))) continue;
				}

				moves[num_moves++] = { origin, square, EMPTY };
			}
		}
		bb = ((bb_pawn & 0x7f7f7f7f7f7f7f7f) >> 7) & (state.occupied_black | state.enpassant) & check_mask;
		while (bb) {
			square = pop_lsb(bb);
			U8 origin = square + 7;

			if (!((1ULL << square) & pin_mask[origin])) continue;

			if (square < 8)
			{
				moves[num_moves++] = { origin, square, WHITE_QUEEN };
				moves[num_moves++] = { origin, square, WHITE_ROOK };
				moves[num_moves++] = { origin, square, WHITE_BISHOP };
				moves[num_moves++] = { origin, square, WHITE_KNIGHT };
			}
			else {
				if (square == state.enpassant) {
					// 앙파상 핀 검사
					U64 from = 1ULL << origin;
					U64 to = 1ULL << square;
					U64 captured = 1ULL << (square + 8);
					if (is_attacked<turn>(king_square, occupied_all ^ (from | to | captured))) continue;
				}

				moves[num_moves++] = { origin, square, EMPTY };
			}
		}
	}
	else {
		// 폰 이동
		U64 bb = (state.bitboards[BLACK_PAWN] << 8) & ~occupied_all;
		U64 doubles = ((bb & 0x0000000000ff0000) << 8) & ~occupied_all & check_mask;
		bb &= check_mask;
		U8 square;
		// 일반 이동
		while (bb) {
			square = pop_lsb(bb);
			U8 origin = square - 8;

			if (!((1ULL << square) & pin_mask[origin])) continue;

			// 프로모션
			if (square >= 56) {
				moves[num_moves++] = { origin, square, BLACK_QUEEN };
				moves[num_moves++] = { origin, square, BLACK_ROOK };
				moves[num_moves++] = { origin, square, BLACK_BISHOP };
				moves[num_moves++] = { origin, square, BLACK_KNIGHT };
			}
			else moves[num_moves++] = { origin, square, EMPTY };
		}
		// 2칸 이동
		while (doubles) {
			square = pop_lsb(doubles);
			U8 origin = square - 16;

			if (!((1ULL << square) & pin_mask[origin])) continue;

			moves[num_moves++] = { origin, square, EMPTY };
		}
		// 폰 공격
		bb = ((state.bitboards[BLACK_PAWN] & 0xfefefefefefefefe) << 7) & (state.occupied_white | state.enpassant) & check_mask;
		while (bb) {
			square = pop_lsb(bb);
			U8 origin = square - 7;

			if (!((1ULL << square) & pin_mask[origin])) continue;

			if (square >= 56)
			{
				moves[num_moves++] = { origin, square, BLACK_QUEEN };
				moves[num_moves++] = { origin, square, BLACK_ROOK };
				moves[num_moves++] = { origin, square, BLACK_BISHOP };
				moves[num_moves++] = { origin, square, BLACK_KNIGHT };
			}
			else {
				if (square == state.enpassant) {
					// 앙파상 핀 검사
					U64 from = 1ULL << origin;
					U64 to = 1ULL << square;
					U64 captured = 1ULL << (square - 8);
					if (is_attacked<turn>(king_square, occupied_all ^ (from | to | captured))) continue;
				}

				moves[num_moves++] = { origin, square, EMPTY };
			}
		}
		bb = ((state.bitboards[BLACK_PAWN] & 0x7f7f7f7f7f7f7f7f) << 9) & (state.occupied_white | state.enpassant) & check_mask;
		while (bb) {
			square = pop_lsb(bb);
			U8 origin = square - 9;

			if (!((1ULL << square) & pin_mask[origin])) continue;

			if (square >= 56)
			{
				moves[num_moves++] = { origin, square, BLACK_QUEEN };
				moves[num_moves++] = { origin, square, BLACK_ROOK };
				moves[num_moves++] = { origin, square, BLACK_BISHOP };
				moves[num_moves++] = { origin, square, BLACK_KNIGHT };
			}
			else {
				if (square == state.enpassant) {
					U64 from = 1ULL << origin;
					U64 to = 1ULL << square;
					U64 captured = 1ULL << (square - 8);
					if (is_attacked<turn>(king_square, occupied_all ^ (from | to | captured))) continue;
				}

				moves[num_moves++] = { origin, square, EMPTY };
			}
		}
	}

	// 나이트 공격
	while (bb_knight) {
		square = pop_lsb(bb_knight);

		U64 knight_moves = knight_attacks[square] & ~occupied_us & check_mask;
		U8 target;
		while (knight_moves) {
			target = pop_lsb(knight_moves);
			moves[num_moves++] = { square, target, EMPTY };
		}
	}

	// 비숍 공격
	while (bb_bishop)
	{
		square = pop_lsb(bb_bishop);

		MagicEntry magics = BISHOP_MAGICS[square];
		U64 relevant = occupied_all & magics.mask;
		int index = (int)((relevant * magics.magic) >> magics.shift);
		U64 attacks = bishop_rays[magics.offset + index];
		attacks &= ~occupied_us & pin_mask[square] & check_mask;

		U8 target;
		while (attacks) {
			target = pop_lsb(attacks);
			moves[num_moves++] = { square, target, EMPTY };
		}
	}

	// 룩 공격
	while (bb_rook)
	{
		square = pop_lsb(bb_rook);

		MagicEntry magics = ROOK_MAGICS[square];
		U64 relevant = occupied_all & magics.mask;
		int index = (int)((relevant * magics.magic) >> magics.shift);
		U64 attacks = rook_rays[magics.offset + index];
		attacks &= ~occupied_us & pin_mask[square] & check_mask;

		U8 target;
		while (attacks) {
			target = pop_lsb(attacks);
			moves[num_moves++] = { square, target, EMPTY };
		}
	}

	// 캐슬링
	if (checked == 0) {
		if constexpr (turn == WHITE) {
			if (state.WKC && !(occupied_all & 0x6000000000000000)
				&& !is_attacked<turn>(G1, occupied_all) && !is_attacked<turn>(F1, occupied_all)) {
				moves[num_moves++] = { E1, G1, EMPTY };
			}
			if (state.WQC && !(occupied_all & 0x0e00000000000000)
				&& !is_attacked<turn>(D1, occupied_all) && !is_attacked<turn>(C1, occupied_all)) {
				moves[num_moves++] = { E1, C1, EMPTY };
			}
		}
		else {
			if (state.BKC && !(occupied_all & 0x0000000000000060)
				&& !is_attacked<turn>(G8, occupied_all) && !is_attacked<turn>(F8, occupied_all)) {
				moves[num_moves++] = { E8, G8, EMPTY };
			}
			if (state.BQC && !(occupied_all & 0x000000000000000e)
				&& !is_attacked<turn>(C8, occupied_all) && !is_attacked<turn>(D8, occupied_all)) {
				moves[num_moves++] = { E8, C8, EMPTY };
			}
		}
	}

	return num_moves;
}

int Board::get_legal_moves(Move* move) {
	if (state.turn == WHITE) {
		return get_legal_moves_impl<WHITE>(move);
	}
	else {
		return get_legal_moves_impl<BLACK>(move);
	}
}

template<bool turn>
bool Board::is_attacked(U8 square, U64 occupied) {
	// 자기자신을 공격하진 않기 때문에 기물 잡힘은 신경 쓰지 않아도 된다.
	// 즉, 단순히 occupied ^= move.from | move.to만 해도 잘 작동한다.

	constexpr U8 KNIGHT = (turn == WHITE) ? BLACK_KNIGHT : WHITE_KNIGHT;
	constexpr U8 BISHOP = (turn == WHITE) ? BLACK_BISHOP : WHITE_BISHOP;
	constexpr U8 ROOK = (turn == WHITE) ? BLACK_ROOK : WHITE_ROOK;
	constexpr U8 QUEEN = (turn == WHITE) ? BLACK_QUEEN : WHITE_QUEEN;
	constexpr U8 KING = (turn == WHITE) ? BLACK_KING : WHITE_KING;

	if (knight_attacks[square] & state.bitboards[KNIGHT]) return true;

	if (king_attacks[square] & state.bitboards[KING]) return true;

	if constexpr (turn == WHITE) {
		if (white_pawn_attacks[square] & state.bitboards[BLACK_PAWN]) return true;
	}
	else {
		if (black_pawn_attacks[square] & state.bitboards[WHITE_PAWN]) return true;
	}

	MagicEntry magics = BISHOP_MAGICS[square];
	U64 relevant = occupied & magics.mask;
	int index = (int)((relevant * magics.magic) >> magics.shift);
	U64 attacks = bishop_rays[magics.offset + index];
	if (attacks & (state.bitboards[BISHOP] | state.bitboards[QUEEN])) return true;

	magics = ROOK_MAGICS[square];
	relevant = occupied & magics.mask;
	index = (int)((relevant * magics.magic) >> magics.shift);
	attacks = rook_rays[magics.offset + index];
	if (attacks & (state.bitboards[ROOK] | state.bitboards[QUEEN])) return true;

	return false;
}

bool Board::is_checked() {
	if (state.turn == WHITE) {
		U8 square = lsb_index(state.bitboards[BLACK_KING]);
		return is_attacked<BLACK>(square, 0);
	}
	else {
		U8 square = lsb_index(state.bitboards[WHITE_KING]);
		return is_attacked<WHITE>(square, 0);
	}
}

void Board::set_position(string fen) {
	stack_index = 0;

	for (int i = 0; i < 12; i++) state.bitboards[i] = 0;

	state.WKC = false;
	state.WQC = false;
	state.BKC = false;
	state.BQC = false;
	state.enpassant = 0;

	stringstream ss(fen);

	string boardString;
	string turn;
	string castling;
	string passant;
	string halfMoves;
	string fullMoves;

	ss >> boardString;
	ss >> turn;
	ss >> castling;
	ss >> passant;
	ss >> halfMoves;
	ss >> fullMoves;

	int x = 0;
	int y = 0;
	for (int i = 0; i < boardString.length(); ++i)
	{
		char c = boardString[i];
		uint8_t square = x + 8 * y;
		if (c == '/')
		{
			x = 0;
			++y;
		}
		else if (c <= '9' && c >= '0') x += c - '0';
		else if (c == 'p')
		{
			set_piece_at(BLACK_PAWN, square);
			++x;
		}
		else if (c == 'n')
		{
			set_piece_at(BLACK_KNIGHT, square);
			++x;
		}
		else if (c == 'b')
		{
			set_piece_at(BLACK_BISHOP, square);
			++x;
		}
		else if (c == 'r')
		{
			set_piece_at(BLACK_ROOK, square);
			++x;
		}
		else if (c == 'q')
		{
			set_piece_at(BLACK_QUEEN, square);
			++x;
		}
		else if (c == 'k')
		{
			set_piece_at(BLACK_KING, square);
			++x;
		}
		else if (c == 'P')
		{
			set_piece_at(WHITE_PAWN, square);
			++x;
		}
		else if (c == 'N')
		{
			set_piece_at(WHITE_KNIGHT, square);
			++x;
		}
		else if (c == 'B')
		{
			set_piece_at(WHITE_BISHOP, square);
			++x;
		}
		else if (c == 'R')
		{
			set_piece_at(WHITE_ROOK, square);
			++x;
		}
		else if (c == 'Q')
		{
			set_piece_at(WHITE_QUEEN, square);
			++x;
		}
		else if (c == 'K')
		{
			set_piece_at(WHITE_KING, square);
			++x;
		}
	}

	state.occupied_white = 0;
	for (int i = 0; i < 6; i++) state.occupied_white |= state.bitboards[i];
	state.occupied_black = 0;
	for (int i = 6; i < 12; i++) state.occupied_black |= state.bitboards[i];
	occupied_all = state.occupied_white | state.occupied_black;

	if (turn == "b") state.turn = BLACK;
	else if (turn == "w") state.turn = WHITE;

	for (int i = 0; i < castling.length(); ++i)
	{
		char c = castling[i];

		if (c == 'K') state.WKC = true;
		else if (c == 'k') state.BKC = true;
		else if (c == 'Q') state.WQC = true;
		else if (c == 'q') state.BQC = true;
	}

	if (passant.length() >= 2)
	{
		int passant_x = passant[0] - 'a';
		int passant_y = 7 - passant[1] + '1';

		U8 passantSquare = passant_x + 8 * passant_y;
		state.enpassant = 1ULL << passantSquare;
	}

	state.half_moves = stoi(halfMoves);

	state.hash = 0;
	for (int i = 0; i < 12; i++) {
		U64 bb = state.bitboards[i];
		while (bb) {
			state.hash ^= board_hash[i][lsb_index(bb)];
			bb &= bb - 1;
		}
	}
	state.hash ^= WKC_hash;
	state.hash ^= WQC_hash;
	state.hash ^= BKC_hash;
	state.hash ^= BQC_hash;
	if (state.enpassant) state.hash ^= enpassant_hash[lsb_index(state.enpassant)];
	if (state.turn == BLACK) state.hash ^= turn_hash;
}

U64 Board::generate_attacks(
	U8 square,
	U64 occupied,
	const int dir_file[],
	const int dir_rank[],
	int dir_count,
	int max_depth
) {
	U64 attacks = 0;

	U8 file = square % 8;
	U8 rank = square / 8;

	for (int i = 0; i < dir_count; i++) {
		int tf = file + dir_file[i];
		int tr = rank + dir_rank[i];

		for (int j = 0; j < max_depth; j++) {
			if (tf < 0 || tf > 7 || tr < 0 || tr > 7) break;

			U64 bb = 1ULL << (tf + 8 * tr);
			attacks |= bb;

			if (occupied & bb) break;

			tf += dir_file[i];
			tr += dir_rank[i];
		}
	}

	return attacks;
}

static U64 index_to_occupancy(int index, int bits, U64 mask) {
	U64 result = 0;
	U64 m = mask;

	for (int i = 0; i < bits; i++) {
		int lsb = pop_lsb(m);
		if (index & (1 << i)) result |= (1ULL << lsb);
	}

	return result;
}

Board::Board() {
	// 보드 상태 초기화
	state_stack = new BoardState[2000];
	stack_index = 0;

	state.bitboards[WHITE_PAWN] = 0x00ff000000000000;
	state.bitboards[WHITE_KNIGHT] = 0x4200000000000000;
	state.bitboards[WHITE_BISHOP] = 0x2400000000000000;
	state.bitboards[WHITE_ROOK] = 0x8100000000000000;
	state.bitboards[WHITE_QUEEN] = 0x0800000000000000;
	state.bitboards[WHITE_KING] = 0x1000000000000000;
	state.bitboards[BLACK_PAWN] = 0x000000000000ff00;
	state.bitboards[BLACK_KNIGHT] = 0x0000000000000042;
	state.bitboards[BLACK_BISHOP] = 0x0000000000000024;
	state.bitboards[BLACK_ROOK] = 0x0000000000000081;
	state.bitboards[BLACK_QUEEN] = 0x0000000000000008;
	state.bitboards[BLACK_KING] = 0x0000000000000010;

	state.occupied_white = 0;
	for (int i = 0; i < 6; i++) state.occupied_white |= state.bitboards[i];
	state.occupied_black = 0;
	for (int i = 6; i < 12; i++) state.occupied_black |= state.bitboards[i];
	occupied_all = state.occupied_white | state.occupied_black;

	state.WKC = true;
	state.WQC = true;
	state.BKC = true;
	state.BQC = true;
	state.turn = WHITE;
	state.enpassant = 0;
	state.half_moves = 0;

	// 해쉬 초기화
	random_device rd;
	mt19937_64 rng(rd());
	uniform_int_distribution<U64> dist(0, numeric_limits<U64>::max());

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 12; j++) {
			board_hash[j][i] = dist(rng);
		}
	}
	for (int i = 0; i < 8; i++) {
		enpassant_hash[i] = dist(rng);
	}
	turn_hash = dist(rng);
	WKC_hash = dist(rng);
	WQC_hash = dist(rng);
	BKC_hash = dist(rng);
	BQC_hash = dist(rng);

	state.hash = 0;
	for (int i = 0; i < 12; i++) {
		U64 bb = state.bitboards[i];
		while (bb) {
			state.hash ^= board_hash[i][lsb_index(bb)];
			bb &= bb - 1;
		}
	}
	state.hash ^= WKC_hash;
	state.hash ^= WQC_hash;
	state.hash ^= BKC_hash;
	state.hash ^= BQC_hash;

	// 공격 테이블 초기화
	const int white_pawn_f[] = { -1, 1 };
	const int white_pawn_r[] = { -1, -1 };
	const int black_pawn_f[] = { -1, 1 };
	const int black_pawn_r[] = { 1, 1 };
	const int knight_f[] = { 1, -1, 1, -1, 2, 2, -2, -2 };
	const int knight_r[] = { 2, 2, -2, -2, 1, -1, 1, -1 };
	const int bishop_f[] = { 1, -1, 1, -1 };
	const int bishop_r[] = { 1, 1, -1, -1 };
	const int rook_f[] = { 0, 0, 1, -1 };
	const int rook_r[] = { 1, -1, 0, 0 };
	const int king_f[] = { 1, 1, 1, 0, 0, -1, -1, -1 };
	const int king_r[] = { 1, 0, -1, 1, -1, 1, 0, -1 };

	for (int i = 0; i < 64; i++) {
		white_pawn_attacks[i] = generate_attacks(i, 0, white_pawn_f, white_pawn_r, 2, 1);
		black_pawn_attacks[i] = generate_attacks(i, 0, black_pawn_f, black_pawn_r, 2, 1);
		knight_attacks[i] = generate_attacks(i, 0, knight_f, knight_r, 8, 1);
		bishop_attacks[i] = generate_attacks(i, 0, bishop_f, bishop_r, 4, 7);
		rook_attacks[i] = generate_attacks(i, 0, rook_f, rook_r, 4, 7);
		king_attacks[i] = generate_attacks(i, 0, king_f, king_r, 8, 1);

		MagicEntry magics = ROOK_MAGICS[i];
		int bits = (int)__popcnt64(magics.mask);
		int size = 1 << bits;

		for (int j = 0; j < size; j++) {
			U64 occupied = index_to_occupancy(j, bits, magics.mask);
			int index = (int)((occupied * magics.magic) >> magics.shift);
			rook_rays[magics.offset + index] = generate_attacks(i, occupied, rook_f, rook_r, 4, 7);
		}

		magics = BISHOP_MAGICS[i];
		bits = (int)__popcnt64(magics.mask);
		size = 1 << bits;

		for (int j = 0; j < size; j++) {
			U64 occupied = index_to_occupancy(j, bits, magics.mask);
			int index = (int)((occupied * magics.magic) >> magics.shift);
			bishop_rays[magics.offset + index] = generate_attacks(i, occupied, bishop_f, bishop_r, 4, 7);
		}
	}

	for (int from = 0; from < 64; from++)
	{
		int fromRank = from / 8;
		int fromFile = from % 8;

		for (int to = 0; to < 64; to++)
		{
			if (from == to)
			{
				between[from][to] = 0ULL;
				continue;
			}

			int toRank = to / 8;
			int toFile = to % 8;

			int dr = toRank - fromRank;
			int df = toFile - fromFile;

			// 같은 rank, file, 혹은 대각선 위에 있는지 확인
			bool aligned = (dr == 0) || (df == 0) || (std::abs(dr) == std::abs(df));

			uint64_t bb = 0ULL;

			if (aligned)
			{
				int stepR = (dr > 0) - (dr < 0);  // -1, 0, 1
				int stepF = (df > 0) - (df < 0);  // -1, 0, 1

				int r = fromRank + stepR;
				int f = fromFile + stepF;

				while (true)
				{
					int sq = r * 8 + f;
					bb |= (1ULL << sq);

					if (sq == to)
						break;

					r += stepR;
					f += stepF;
				}
			}

			between[from][to] = bb;
		}
	}
}
