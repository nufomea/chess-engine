#include "board.h"
#include <iostream>
#include <chrono>

using namespace std;

static constexpr const char* squareNames[64] = {
	"A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8",
	"A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
	"A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
	"A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
	"A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
	"A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
	"A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
	"A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1"
};

Board board;

static U64 perft(int depth)
{
	if (depth == 0)
		return 1ULL;

	Move moves[218];
	int num_moves = board.get_legal_moves(moves);

	U64 nodes = 0;

	for (int i = 0; i < num_moves; i++)
	{
		board.push(moves[i]);
		nodes += perft(depth - 1);
		board.pop();
	}

	return nodes;
}

void divide(int depth)
{
	Move moves[218];
	int num_moves = board.get_legal_moves(moves);

	U64 total = 0;

	for (int i = 0; i < num_moves; ++i)
	{
		board.push(moves[i]);

		U64 nodes = perft(depth - 1);

		board.pop();

		cout
			<< squareNames[moves[i].from]
			<< squareNames[moves[i].to]
			<< ": "
			<< nodes
			<< '\n';

		total += nodes;
	}

	cout << "Total: " << total << '\n';
}

int main() {

	auto start = std::chrono::high_resolution_clock::now();
	U64 nodes = perft(7);
	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
	double seconds = elapsed_seconds.count();

	double nps = (seconds > 0) ? (nodes / seconds) : 0;

	std::cout << "Visited nodes: " << nodes << "\n";
	std::cout << "Time elapsed : " << seconds << " s\n";
	std::cout << "NPS          : " << static_cast<uint64_t>(nps) << " nodes/s\n";

	return 0;
}
