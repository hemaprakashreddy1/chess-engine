#pragma once

#include <string>
#include <vector>
#include <array>

#include "Move.h"
#include "StateInfo.h"

class Position {
    public:
    std::array<int, 64> board;
    int turn;
    int half_move_cnt;
    int full_move_cnt;
    int en_passant_square;
    int castle_rights;
    int white_king_square;
    int black_king_square;
    std::vector<Move> moves;
    
    std::vector<std::vector<int>> edges;
    std::vector<StateInfo> states;

    Position(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    int get_color(int piece);

    int get_type(int piece);

    int square(int rank, int file);

    int get_rank(int square);

    int get_file(int square);

    void load_game_from_fen(const std::string &fen);

    std::string generate_fen();

    void print_board();

    void print();

    void make_move(Move &move);

    void unmake_move();

    void generate_steps_to_edges();

    bool is_square_attacked(int square, int attacker_color);

    std::vector<int> generate_knight_squares(int square);

    std::vector<Move> generate_straight_moves(int square, int color, int steps_limit = 8);

    std::vector<Move> generate_cross_moves(int square, int color, int steps_limit = 8);

    std::vector<Move> generate_pawn_moves(int square, int color);

    std::vector<Move> generate_knight_moves(int square, int color);

    std::vector<Move> generate_castle_moves(int square, int color);

    std::vector<Move> generate_pseudo_legal_moves(int color);

    std::vector<Move> generate_moves(int color);

    std::string square_to_notation(int square);

    int notation_to_square(std::string s);

    long long perft(int depth, bool first_step = true);

    void print_fen_after_move(std::string move);
};