import chess

from dataclasses import dataclass

@dataclass
class Limit:
    time: float = None
    depth: int = None
    nodes: int = None


class Engine:
    def __init__(self):
        self.PIECE_VALUE = {
            chess.PAWN: 100,
            chess.KNIGHT: 300,
            chess.BISHOP: 300,
            chess.ROOK: 500,
            chess.QUEEN: 900,
            chess.KING: 0
        }
        
        self.PST = {
            chess.PAWN: [
                0,   0,   0,   0,   0,   0,   0,   0,
                78,  83,  86,  73, 102,  82,  85,  90,
                7,  29,  21,  44,  40,  31,  44,   7,
                -17,  16,  -2,  15,  14,   0,  15, -13,
                -26,   3,  10,   9,   6,   1,   0, -23,
                -22,   9,   5, -11, -10,  -2,   3, -19,
                -31,   8,  -7, -37, -36, -14,   3, -31,
                0,   0,   0,   0,   0,   0,   0,   0
            ],
            chess.KNIGHT: [
                -66, -53, -75, -75, -10, -55, -58, -70,
                -3,  -6, 100, -36,   4,  62,  -4, -14,
                10,  67,   1,  74,  73,  27,  62,  -2,
                24,  24,  45,  37,  33,  41,  25,  17,
                -1,   5,  31,  21,  22,  35,   2,   0,
                -18,  10,  13,  22,  18,  15,  11, -14,
                -23, -15,   2,   0,   2,   0, -23, -20,
                -74, -23, -26, -24, -19, -35, -22, -69
            ],
            chess.BISHOP: [
                -59, -78, -82, -76, -23,-107, -37, -50,
                -11,  20,  35, -42, -39,  31,   2, -22,
                -9,  39, -32,  41,  52, -10,  28, -14,
                25,  17,  20,  34,  26,  25,  15,  10,
                13,  10,  17,  23,  17,  16,   0,   7,
                14,  25,  24,  15,   8,  25,  20,  15,
                19,  20,  11,   6,   7,   6,  20,  16,
                -7,   2, -15, -12, -14, -15, -10, -10
            ],
            chess.ROOK: [
                35,  29,  33,   4,  37,  33,  56,  50,
                55,  29,  56,  67,  55,  62,  34,  60,
                19,  35,  28,  33,  45,  27,  25,  15,
                0,   5,  16,  13,  18,  -4,  -9,  -6,
                -28, -35, -16, -21, -13, -29, -46, -30,
                -42, -28, -42, -25, -25, -35, -26, -46,
                -53, -38, -31, -26, -29, -43, -44, -53,
                -30, -24, -18,   5,  -2, -18, -31, -32
            ],
            chess.QUEEN: [
                6,   1,  -8,-104,  69,  24,  88,  26,
                14,  32,  60, -10,  20,  76,  57,  24,
                -2,  43,  32,  60,  72,  63,  43,   2,
                1, -16,  22,  17,  25,  20, -13,  -6,
                -14, -15,  -2,  -5,  -1, -10, -20, -22,
                -30,  -6, -13, -11, -16, -11, -16, -27,
                -36, -18,   0, -19, -15, -15, -21, -38,
                -39, -30, -31, -13, -31, -36, -34, -42
            ],
            chess.KING: [
                4,  54,  47, -99, -99,  60,  83, -62,
                -32,  10,  55,  56,  56,  55,  10,   3,
                -62,  12, -57,  44, -67,  28,  37, -31,
                -55,  50,  11,  -4, -19,  13,   0, -49,
                -55, -43, -52, -28, -51, -47,  -8, -50,
                -47, -42, -43, -79, -64, -32, -29, -32,
                -4,   3, -14, -50, -57, -18,  13,   4,
                17,  30,  -3, -14,   6,  -1,  40,  18
            ],
        }
        
        for piece, table in self.PST.items():
            for i in range(64):
                self.PST[piece][i] += self.PIECE_VALUE[piece]
        
        self.CHECKMATE_SCORE = 10_000

    def _evaluate(self, board: chess.Board) -> int:
        score = 0
        
        if board.turn is chess.BLACK:
            temp_board = board.mirror()
        else:
            temp_board = board
        
        for square, piece in temp_board.piece_map().items():
            if piece.color is chess.WHITE:
                score += self.PST[piece.piece_type][square]
            else:
                score -= self.PST[piece.piece_type][square]
        
        return score
    
    def _material_score(self, board: chess.Board) -> int:
        return (
            100 * (
                chess.popcount(board.pawns & board.occupied_co[chess.WHITE]) -
                chess.popcount(board.pawns & board.occupied_co[chess.BLACK])
            )
            + 300 * (
                chess.popcount(board.knights & board.occupied_co[chess.WHITE]) -
                chess.popcount(board.knights & board.occupied_co[chess.BLACK])
            )
            + 300 * (
                chess.popcount(board.bishops & board.occupied_co[chess.WHITE]) -
                chess.popcount(board.bishops & board.occupied_co[chess.BLACK])
            )
            + 500 * (
                chess.popcount(board.rooks & board.occupied_co[chess.WHITE]) -
                chess.popcount(board.rooks & board.occupied_co[chess.BLACK])
            )
            + 900 * (
                chess.popcount(board.queens & board.occupied_co[chess.WHITE]) -
                chess.popcount(board.queens & board.occupied_co[chess.BLACK])
            )
        )
    
    def move_ordering(self, board: chess.Board):
        def move_score(move: chess.Move):
            score = 0
            
            if board.is_en_passant(move):
                score += 100
            elif board.is_capture(move):
                victim = board.piece_at(move.to_square).piece_type
                attacker = board.piece_at(move.from_square).piece_type
                score += self.PIECE_VALUE[victim] * 2 - self.PIECE_VALUE[attacker]
            
            if move.promotion:
                score += self.PIECE_VALUE[move.promotion]
            
            return score

        return sorted(board.legal_moves, key=move_score, reverse=True)
    
    def _negamax(self, board: chess.Board, depth, alpha, beta):
        self.nodes += 1
        
        if board.is_checkmate():
            return -self.CHECKMATE_SCORE - depth
        elif board.is_fifty_moves() or board.is_insufficient_material() or board.is_stalemate() or board.is_repetition():
            return 0
        
        if depth <= 0:
            return self._evaluate(board)
        
        best_score = float('-inf')
        
        for move in self.move_ordering(board):
            board.push(move)
            score = -self._negamax(board, depth - 1, -beta, -alpha)
            board.pop()
            
            if score > best_score:
                best_score = score
            
            alpha = max(alpha, score)
            if alpha >= beta:
                break
        
        return best_score

    def search(self, board: chess.Board, limit: Limit):
        self.limit = limit
        
        self.nodes = 0
        
        moves = list(board.legal_moves)
        move_scores = {move: 0 for move in moves}
        
        best_move = None
        best_score = float('-inf')
        
        for current_depth in range(1, self.limit.depth + 1):
            moves.sort(key=lambda m: move_scores[m], reverse=True)
            
            current_best_move = None
            current_best_score = float('-inf')
            
            alpha = float('-inf')
            beta = float('inf')
            
            for move in moves:
                board.push(move)
                score = -self._negamax(board, current_depth - 1, -beta, -alpha)
                board.pop()
                
                move_scores[move] = score
                
                if score > current_best_score:
                    current_best_score = score
                    current_best_move = move
                
                alpha = max(alpha, score)
                if alpha >= beta:
                    break
            
            if current_best_move:
                best_move = current_best_move
                best_score = current_best_score
        
        return best_move, best_score


import time
import random
from tqdm import tqdm

engine = Engine()

times = []
nodes = []
npss = []

board = chess.Board()
for i in range(5):
    for i in range(random.randint(1, 15)):
        if board.is_game_over():
            board = chess.Board()
        board.push(random.choice(list(board.legal_moves)))
    
    s = time.perf_counter()
    move, score = engine.search(board, Limit(depth=5))
    e = time.perf_counter()
    
    consum = e - s
    nps = engine.nodes / consum if consum > 0 else 0
    times.append(consum)
    npss.append(nps)
    nodes.append(engine.nodes)
    
    print(f'탐색 시간: {consum} | 탐색 노드: {engine.nodes} | NPS: {nps}')
    print(f'최선수: {move} | 점수: {score}')

print(f'Time: {sum(times) / len(times):.10f} | NPS: {sum(npss) / len(npss):.2f} | Nodes: {sum(nodes) / len(nodes):.2f}')
