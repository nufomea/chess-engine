#include "board.h"
#include <iostream>
#include <chrono>
#include <windows.h>
#include <vector>
#include <tlhelp32.h>

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

// P-core의 CPU Set ID 목록을 수집하는 함수
std::vector<ULONG> GetPCoreCpuSetIds() {
	std::vector<ULONG> pCoreIds;
	ULONG returnLength = 0;

	// 1. 필요한 버퍼 크기 조회
	GetSystemCpuSetInformation(nullptr, 0, &returnLength, GetCurrentProcess(), 0);
	if (returnLength == 0) return pCoreIds;

	std::vector<BYTE> buffer(returnLength);
	SYSTEM_CPU_SET_INFORMATION* cpuSetInfo = reinterpret_cast<SYSTEM_CPU_SET_INFORMATION*>(buffer.data());

	// 2. 전체 CPU Set 정보 가져오기
	if (GetSystemCpuSetInformation(cpuSetInfo, returnLength, &returnLength, GetCurrentProcess(), 0)) {
		DWORD offset = 0;
		while (offset < returnLength) {
			SYSTEM_CPU_SET_INFORMATION* current = reinterpret_cast<SYSTEM_CPU_SET_INFORMATION*>(reinterpret_cast<BYTE*>(cpuSetInfo) + offset);

			if (current->Type == CpuSetInformation) {
				// EfficiencyClass: 숫자가 높을수록 고성능 코어(P-core)입니다.
				// 0은 보통 E-core, 1 이상이 P-core에 해당합니다.
				if (current->CpuSet.EfficiencyClass > 0) {
					pCoreIds.push_back(current->CpuSet.Id);
				}
			}
			offset += current->Size;
		}
	}
	return pCoreIds;
}

// 특정 스레드를 P-core에 고정하는 함수
bool PinThreadToPCores(HANDLE hThread) {
	std::vector<ULONG> pCoreIds = GetPCoreCpuSetIds();

	if (pCoreIds.empty()) {
		std::cerr << "P-core를 찾을 수 없거나 구형 시스템입니다.\n";
		return false;
	}

	// 스레드에 P-core CpuSet ID 배열 지정
	BOOL result = SetThreadSelectedCpuSets(
		hThread,
		pCoreIds.data(),
		static_cast<ULONG>(pCoreIds.size())
	);

	return result != 0;
}

size_t GetCurrentThreadCount() {
	DWORD currentPid = GetCurrentProcessId();
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

	THREADENTRY32 te;
	te.dwSize = sizeof(THREADENTRY32);
	size_t threadCount = 0;

	if (Thread32First(hSnapshot, &te)) {
		do {
			if (te.th32OwnerProcessID == currentPid) {
				threadCount++;
			}
		} while (Thread32Next(hSnapshot, &te));
	}

	CloseHandle(hSnapshot);
	return threadCount;
}

int main() {

	auto start = std::chrono::high_resolution_clock::now();
	U64 nodes = perft(6);
	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
	double seconds = elapsed_seconds.count();

	double nps = (seconds > 0) ? (nodes / seconds) : 0;

	std::cout << "Visited nodes: " << nodes << "\n";
	std::cout << "Time elapsed : " << seconds << " s\n";
	std::cout << "NPS          : " << static_cast<uint64_t>(nps) << " nodes/s\n";

	return 0;
}
