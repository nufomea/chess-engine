// magic_bitboard.cpp
// 룩(Rook) 또는 비숍(Bishop) 중 하나와 칸(0~63)을 선택하여
// 해당 기물/칸에 대한 매직 넘버를 탐색하고,
// 매직 넘버, 마스크, shift, 테이블 크기, 그리고
// 탐색 과정에서 발생한 "무해한 해쉬 충돌"
// (인덱스는 겹치지만 공격 범위(attack) 값이 동일해서 그냥 넘어간 경우)의
// 위치와 값을 출력한다.

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <cstdint>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

using namespace std;

typedef unsigned long long U64;

// ------------------------- 기본 유틸 -------------------------
// popcount / 최하위 비트(LSB) 추출은 컴파일러마다 내장 함수 이름이 달라서
// MSVC(Visual Studio)와 GCC/Clang을 모두 지원하도록 분기 처리한다.

static inline int popcount(U64 b) {
#if defined(_MSC_VER)
    return (int)__popcnt64(b);
#else
    return __builtin_popcountll(b);
#endif
}

// 비트보드에서 최하위 비트(LSB)의 인덱스를 꺼내고 그 비트를 제거한다.
static inline int pop_lsb(U64& b) {
#if defined(_MSC_VER)
    unsigned long idx;
    _BitScanForward64(&idx, b);
    b &= b - 1;
    return (int)idx;
#else
    int idx = __builtin_ctzll(b);
    b &= b - 1;
    return idx;
#endif
}

static inline bool onBoard(int r, int f) {
    return r >= 0 && r <= 7 && f >= 0 && f <= 7;
}

static inline U64 sqBit(int r, int f) {
    return 1ULL << (r * 8 + f);
}

// 64비트 난수 (실제 매직 넘버 후보 생성에 사용)
static U64 randState = 88172645463325252ULL; // xorshift 시드
static inline U64 randU64() {
    randState ^= randState << 13;
    randState ^= randState >> 7;
    randState ^= randState << 17;
    return randState;
}

// 좋은 매직 넘버 후보는 대체로 1비트가 적은(sparse) 수일 때
// 잘 나온다는 경험칙에 따라, 여러 난수를 AND 하여 후보를 만든다.
static inline U64 randMagicCandidate() {
    return randU64() & randU64() & randU64();
}

// ------------------------- 마스크 생성 -------------------------
// 마스크는 "테두리(rank 0,7 / file 0,7)를 제외한" 관련 칸들이다.
// 테두리 칸의 점유 여부는 어차피 슬라이딩이 보드 끝에서 멈추므로
// 결과에 영향을 주지 않기 때문에 제외한다.

U64 rookMask(int sq) {
    int r = sq / 8, f = sq % 8;
    U64 mask = 0ULL;
    for (int rr = r + 1; rr <= 6; rr++) mask |= sqBit(rr, f);
    for (int rr = r - 1; rr >= 1; rr--) mask |= sqBit(rr, f);
    for (int ff = f + 1; ff <= 6; ff++) mask |= sqBit(r, ff);
    for (int ff = f - 1; ff >= 1; ff--) mask |= sqBit(r, ff);
    return mask;
}

U64 bishopMask(int sq) {
    int r = sq / 8, f = sq % 8;
    U64 mask = 0ULL;
    for (int rr = r + 1, ff = f + 1; rr <= 6 && ff <= 6; rr++, ff++) mask |= sqBit(rr, ff);
    for (int rr = r + 1, ff = f - 1; rr <= 6 && ff >= 1; rr++, ff--) mask |= sqBit(rr, ff);
    for (int rr = r - 1, ff = f + 1; rr >= 1 && ff <= 6; rr--, ff++) mask |= sqBit(rr, ff);
    for (int rr = r - 1, ff = f - 1; rr >= 1 && ff >= 1; rr--, ff--) mask |= sqBit(rr, ff);
    return mask;
}

// ------------------------- 실제 공격 범위 계산 -------------------------
// 주어진 점유 비트보드(occ, 보드 전체 기준)에 대해
// 첫 번째 막힌 칸까지(막힌 칸 포함) 공격 범위를 계산한다.

U64 rookAttacksOnFly(int sq, U64 occ) {
    U64 att = 0ULL;
    int r = sq / 8, f = sq % 8;
    int dr[4] = { 1, -1, 0, 0 };
    int df[4] = { 0, 0, 1, -1 };
    for (int d = 0; d < 4; d++) {
        int rr = r + dr[d], ff = f + df[d];
        while (onBoard(rr, ff)) {
            U64 b = sqBit(rr, ff);
            att |= b;
            if (occ & b) break;
            rr += dr[d]; ff += df[d];
        }
    }
    return att;
}

U64 bishopAttacksOnFly(int sq, U64 occ) {
    U64 att = 0ULL;
    int r = sq / 8, f = sq % 8;
    int dr[4] = { 1, 1, -1, -1 };
    int df[4] = { 1, -1, 1, -1 };
    for (int d = 0; d < 4; d++) {
        int rr = r + dr[d], ff = f + df[d];
        while (onBoard(rr, ff)) {
            U64 b = sqBit(rr, ff);
            att |= b;
            if (occ & b) break;
            rr += dr[d]; ff += df[d];
        }
    }
    return att;
}

// index(0 ~ 2^bits-1) 를 mask 의 부분집합(occupancy)으로 변환한다.
U64 indexToOccupancy(int index, int bits, U64 mask) {
    U64 result = 0ULL;
    U64 m = mask;
    for (int i = 0; i < bits; i++) {
        int lsb = pop_lsb(m);
        if (index & (1 << i)) result |= (1ULL << lsb);
    }
    return result;
}

// ------------------------- 결과 구조체 -------------------------

struct Collision {
    int index;       // 충돌이 발생한 테이블 인덱스
    U64 occupancy;   // 그 인덱스를 만들어낸 점유 비트보드
    U64 attack;       // 해당 인덱스에 저장된(동일했던) 공격 범위 값
};

struct MagicResult {
    U64 magic;
    U64 mask;
    int shift;
    int tableSize = 0;
    vector<Collision> collisions; // "값이 같아서 그냥 넘어간" 무해한 충돌들
};

// ------------------------- 매직 넘버 탐색 -------------------------

MagicResult findMagic(int sq, bool isRook) {
    U64 mask = isRook ? rookMask(sq) : bishopMask(sq);
    int bits = popcount(mask);
    int size = 1 << bits;
    int shift = 64 - bits;

    vector<U64> occupancies(size);
    vector<U64> attacks(size);
    for (int i = 0; i < size; i++) {
        occupancies[i] = indexToOccupancy(i, bits, mask);
        attacks[i] = isRook ? rookAttacksOnFly(sq, occupancies[i])
            : bishopAttacksOnFly(sq, occupancies[i]);
    }

    vector<U64> used(size);
    vector<char> filled(size);
    MagicResult result;

    while (true) {
        U64 magic = randMagicCandidate();

        // 상위 8비트에 세워진 비트 수가 너무 적으면 좋은 후보가 아닐
        // 확률이 높다는 경험적 필터 (탐색 속도 향상용, 정확성과 무관)
        if (popcount((mask * magic) >> 56) < 6) continue;

        fill(used.begin(), used.end(), 0ULL);
        fill(filled.begin(), filled.end(), 0);

        vector<Collision> collisions;
        bool fail = false;

        for (int i = 0; i < size; i++) {
            int idx = (int)((occupancies[i] * magic) >> shift);
            if (!filled[idx]) {
                filled[idx] = 1;
                used[idx] = attacks[i];
            }
            else if (used[idx] == attacks[i]) {
                // 인덱스는 겹쳤지만 attack 값이 동일하므로
                // 실제로는 문제가 되지 않는 "무해한 충돌" -> 기록하고 계속 진행
                collisions.push_back({ idx, occupancies[i], attacks[i] });
            }
            else {
                // attack 값까지 다르면 이 매직 넘버는 사용할 수 없음
                fail = true;
                break;
            }
        }

        if (!fail) {
            result.magic = magic;
            result.mask = mask;
            result.shift = shift;
            result.tableSize = size;
            result.collisions = collisions;
            return result;
        }
    }
}

// ------------------------- 출력 유틸 -------------------------

void printBitboard(U64 b) {
    for (int r = 7; r >= 0; r--) {
        for (int f = 0; f < 8; f++) {
            cout << ((b & sqBit(r, f)) ? "1 " : ". ");
        }
        cout << "\n";
    }
}

string squareName(int sq) {
    string s;
    s += char('a' + sq % 8);
    s += char('1' + sq / 8);
    return s;
}

// ------------------------- main -------------------------
