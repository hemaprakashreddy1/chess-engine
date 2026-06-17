#include <iostream>
#include <vector>
#include <sstream>
#include <array>

namespace Color {
    const int WHITE = 8, BLACK = 16, COLOR_MASK = 24;
}

namespace PieceType {
    const int NONE = 0, KING = 1, QUEEN = 2, ROOK = 3, BISHOP = 4, KNIGHT = 5, PAWN = 6, TYPE_MASK = 7;
}

namespace Castle {
    const int WHITE_KING = 8, WHITE_QUEEN = 4, BLACK_KING = 2, BLACK_QUEEN = 1;
}

enum class MoveType {
    NORMAL, KING_CASTLE, QUEEN_CASTLE, PROMOTION, EN_PASSANT
};

enum class GameResult {
    NO_RESULT, DRAW, WHITE_WON, BLACK_WON
};

const int NORTH = 8;
const int SOUTH = -8;
const int EAST  = 1;
const int WEST  = -1;
const int NORTH_EAST = 9;
const int NORTH_WEST = 7;
const int SOUTH_EAST = -7;
const int SOUTH_WEST = -9;
const int DIRECTION_OFFSETS[] = {NORTH, EAST, WEST, SOUTH, NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST};
const std::vector<int> PROMOTION_TYPES = {PieceType::QUEEN, PieceType::ROOK, PieceType::BISHOP, PieceType::KNIGHT};

struct StateInfo {
    int half_move_cnt;
    int castle_rights;
    int en_passant_square;

    StateInfo(int half_cnt, int castle, int en_passant) {
        half_move_cnt = half_cnt;
        castle_rights = castle;
        en_passant_square = en_passant;
    }
};

struct Move {
    int from;
    int to;
    int capture_position;
    int from_piece;
    int to_piece;
    int captured_piece;
    MoveType move_type;

    Move() {

    }

    Move(int _from, int _to, int _capture_position, int _from_piece, int _to_piece, int _captured_piece, MoveType _move_type) {
        from = _from;
        to = _to;
        capture_position = _capture_position;
        from_piece = _from_piece;
        to_piece = _to_piece;
        captured_piece = _captured_piece;
        move_type = _move_type;
    }
};

struct Game {
    std::array<int, 64> board;
    int turn;
    int half_move_cnt;
    int full_move_cnt;
    int en_passant_square;
    int castle_rights;
    int white_king_square;
    int black_king_square;
    std::vector<Move> moves;
    std::vector<StateInfo> states;

    std::vector<std::vector<int>> edges;
    Game(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
        std::fill(board.begin(), board.end(), 0);
        load_game_from_fen(fen);
        edges.resize(64, std::vector<int> (8));
        generate_steps_to_edges();
    }

    int get_color(int piece) {
        return piece & Color::COLOR_MASK;
    }

    int get_type(int piece) {
        return piece & PieceType::TYPE_MASK;
    }

    int square(int rank, int file) {
        return rank * 8 + file;
    }

    int get_rank(int square) {
        return square / 8;
    }

    int get_file(int square) {
        return square % 8;
    }

    void load_game_from_fen(const std::string &fen) {
        std::stringstream ss(fen);
        int rank = 7, file = 0;

        std::vector<int> pieces(26);
        pieces['r' - 'a'] = PieceType::ROOK;
        pieces['n' - 'a'] = PieceType::KNIGHT;
        pieces['b' - 'a'] = PieceType::BISHOP;
        pieces['q' - 'a'] = PieceType::QUEEN;
        pieces['k' - 'a'] = PieceType::KING;
        pieces['p' - 'a'] = PieceType::PAWN;
        std::string token;
        ss >> token;
        for (char c : token) {
            if (c >= 'a' && c <= 'z') {
                board[square(rank, file)] = Color::BLACK | pieces[c - 'a'];
                if (c == 'k') {
                    black_king_square = square(rank, file);
                }
                file++;
            } else if (c >= 'A' && c <= 'Z') {
                board[square(rank, file)] = Color::WHITE | pieces[c - 'A'];
                if (c == 'K') {
                    white_king_square = square(rank, file);
                }
                file++;
            } else if (c >= '1' && c <= '9') {
                file += c - '0';
            } else if (c == '/') {
                rank--;
                file = 0;
            }
        }

        ss >> token;
        if (token == "w") {
            turn = Color::WHITE;
        } else {
            turn = Color::BLACK;
        }

        ss >> token;
        castle_rights = 0;
        for (char c : token) {
            if (c == 'K') {
                castle_rights |= Castle::WHITE_KING;
            } else if (c == 'Q') {
                castle_rights |= Castle::WHITE_QUEEN;
            } else if (c == 'k') {
                castle_rights |= Castle::BLACK_KING;
            } else if (c == 'q') {
                castle_rights |= Castle::BLACK_QUEEN;
            }
        }

        ss >> token;
        en_passant_square = -1;
        if (token != "-") {
            en_passant_square = (token[1] - '0' - 1) * 8 + token[0] - 'a';
        }

        ss >> half_move_cnt;
        ss >> full_move_cnt;
    }

    std::string generate_fen() {
        std::string fen;
        std::string piece_types = " kqrbnp";
        for (int rank = 7; rank >= 0; rank--) {
            int empty = 0;
            for (int file = 0; file < 8; file++) {
                int piece = board[rank * 8 + file];
                int piece_type = get_type(piece);
                if (piece_type == PieceType::NONE) {
                    empty++;
                } else {
                    if (empty) {
                        fen += char(empty + '0');
                        empty = 0;
                    }
                    int color = get_color(piece);
                    if (color == Color::WHITE) {
                        fen += char(piece_types[piece_type] - 'a' + 'A');
                    } else {
                        fen += piece_types[piece_type];
                    }
                }
            }
            if (empty) {
                fen += char(empty + '0');
            }
            if (rank > 0) {
                fen += '/';
            }
        }
        fen += ' ';

        if (turn == Color::WHITE) {
            fen += 'w';
        } else {
            fen += 'b';
        }
        fen += ' ';

        std::string castle;
        std::string all = "KQkq";
        std::vector<int> masks = {4, 8, 1, 2};
        for (int i = 0; i < 4; i++) {
            if (castle_rights & masks[i]) {
                castle += all[i];
            }
        }
        if (castle.size()) {
            fen += castle;
        } else {
            fen += '-';
        }
        fen += ' ';

        if (en_passant_square == -1) {
            fen += '-';
        } else {
            int rank = get_rank(en_passant_square);
            int file = get_file(en_passant_square);
            fen += char(file + 'a');
            fen += char(rank + '0');
        }
        fen += ' ';

        fen += char(half_move_cnt + '0');
        fen += ' ';

        fen += char(full_move_cnt + '0');

        return fen;
    }

    void print_board() {
        std::vector<char> piece_type_char(7);
        piece_type_char[PieceType::NONE] = ' ';
        piece_type_char[PieceType::KING] = 'k';
        piece_type_char[PieceType::QUEEN] = 'q';
        piece_type_char[PieceType::ROOK] = 'r';
        piece_type_char[PieceType::BISHOP] = 'b';
        piece_type_char[PieceType::KNIGHT] = 'n';
        piece_type_char[PieceType::PAWN] = 'p';

        printf("\t");
        for(int i = 0; i < 8; i++)
        {
            printf("%c\t", char(i + 'a'));
        }
        printf("\n\n\n");
        for(int rank = 7; rank >= 0; rank--)
        {
            printf("%d\t", rank + 1);
            for(int file = 0; file < 8; file++)
            {
                int piece = board[rank * 8 + file];
                int color = get_color(piece);
                int piece_type = get_type(piece);
                char c = piece_type_char[piece_type];
                if (color == Color::WHITE) {
                    c = char('A' + c - 'a');
                }
                printf("%c\t", c);
            }
            printf("\n\n");
        }
    }

    void print() {
        print_board();
        std::cout << "turn " << turn << "\n";
        std::cout << "castle rights " << castle_rights << "\n";
        std::cout << en_passant_square << "\n";
        std::cout << "half moves " << half_move_cnt << "\n";
        std::cout << "full moves " << full_move_cnt << "\n";
        std::cout << "white king square " << white_king_square << "\n";
        std::cout << "black king square " << black_king_square << "\n";
    }

    void make_move(Move &move) {
        states.push_back(StateInfo(half_move_cnt, castle_rights, en_passant_square));
        en_passant_square = -1;
        int type = get_type(move.from_piece);
        if (type == PieceType::KING) {
            if (turn == Color::WHITE) {
                white_king_square = move.to;
                castle_rights = castle_rights & 3;
            } else {
                black_king_square = move.to;
                castle_rights = castle_rights & 12;
            }
        } else if (type == PieceType::ROOK) {
            if (turn == Color::WHITE) {
                if ((castle_rights & 8) && move.from == 0) {
                    castle_rights &= 11;
                } else if ((castle_rights & 4) && move.from == 7) {
                    castle_rights &= 7;
                }
            } else {
                if ((castle_rights & 2) && move.from == 56) {
                    castle_rights &= 14;
                } else if ((castle_rights & 4) && move.from == 63) {
                    castle_rights &= 13;
                }
            }
        } else if (type == PieceType::PAWN) {
            if (turn == Color::WHITE) {
                if (move.from + 16 == move.to) {
                    en_passant_square = move.from + 8;
                }
            } else {
                if (move.from - 16 == move.to) {
                    en_passant_square = move.to - 8;
                }
            }
        }

        if (move.move_type == MoveType::KING_CASTLE) {
            if (turn == Color::WHITE) {
                board[5] = board[7];
                board[7] = 0;
                board[6] = board[4];
                board[4] = 0;
            } else {
                board[61] = board[63];
                board[63] = 0;
                board[62] = board[60];
                board[60] = 0;
            }
        } else if (move.move_type == MoveType::QUEEN_CASTLE) {
            if (turn == Color::WHITE) {
                board[3] = board[0];
                board[0] = 0;
                board[2] = board[4];
                board[4] = 0;
            } else {
                board[59] = board[56];
                board[56] = 0;
                board[58] = board[60];
                board[60] = 0;
            }
        } else {
            board[move.from] = 0;
            board[move.capture_position] = 0;
            board[move.to] = move.to_piece;
        }

        if (turn == Color::BLACK) {
            turn = Color::WHITE;
            full_move_cnt++;
        } else {
            turn = Color::BLACK;
        }
        if (type == PieceType::PAWN || move.captured_piece != PieceType::NONE) {
            half_move_cnt = 0;
        } else {
            half_move_cnt++;
        }
        moves.push_back(move);
    }

    void unmake_move() {
        if (states.size() == 0) {
            std::cerr << "unmake_move() : states is empty\n";
        }
        StateInfo state_info = states.back();
        states.pop_back();
        half_move_cnt = state_info.half_move_cnt;
        castle_rights = state_info.castle_rights;
        en_passant_square = state_info.en_passant_square;

        Move last_move = moves.back();
        moves.pop_back();
        int color = get_color(last_move.from_piece);
        if (color == Color::BLACK) {
            full_move_cnt--;
            turn = Color::BLACK;
        } else {
            turn = Color::WHITE;
        }
        if (get_type(last_move.from_piece) == PieceType::KING) {
            if (color == Color::WHITE) {
                white_king_square = last_move.from;
            } else {
                black_king_square = last_move.from;
            }
        }
        if (last_move.move_type == MoveType::KING_CASTLE) {
            if (turn == Color::WHITE) {
                board[7] = board[5];
                board[5] = 0;
                board[4] = board[6];
                board[6] = 0;
            } else {
                board[63] = board[61];
                board[61] = 0;
                board[60] = board[62];
                board[62] = 0;
            }
        } else if (last_move.move_type == MoveType::QUEEN_CASTLE) {
            if (turn == Color::WHITE) {
                board[0] = board[3];
                board[3] = 0;
                board[4] = board[2];
                board[2] = 0;
            } else {
                board[56] = board[59];
                board[59] = 0;
                board[60] = board[58];
                board[58] = 0;
            }
        } else {
            board[last_move.to] = 0;
            board[last_move.from] = last_move.from_piece;
            board[last_move.capture_position] = last_move.captured_piece;
        }
    }

    void generate_steps_to_edges()
    {
        for(int rank = 0; rank <= 7; rank++)
        {
            for(int file = 0; file <= 7; file++)
            {
                int north = 7 - rank;
                int east = 7 - file;
                int west = file;
                int south = rank;
                int north_east = std::min(north, east);
                int north_west = std::min(north, west);
                int south_east = std::min(south, east);
                int south_west = std::min(south, west);
                int index = rank * 8 + file;
                edges[index][0] = north;
                edges[index][1] = east;
                edges[index][2] = west;
                edges[index][3] = south;
                edges[index][4] = north_east;
                edges[index][5] = north_west;
                edges[index][6] = south_east;
                edges[index][7] = south_west;
            }
        }
    }

    bool is_square_attacked(int square, int attacker_color) {
        //straight attack
        for (int dir = 0; dir < 4; dir++) {
            int direction_offset = DIRECTION_OFFSETS[dir];
            int steps = edges[square][dir];
            for (int i = 0, cur_square = square; i < steps; i++) {
                cur_square += direction_offset;
                int piece = board[cur_square];
                if (piece != 0) {
                    if (get_color(piece) == attacker_color) {
                        int piece_type = get_type(piece);
                        if (piece_type == PieceType::ROOK || piece_type == PieceType::QUEEN || (piece_type == PieceType::KING && i == 0)) {
                            return true;
                        }
                    }
                    break;
                }
            }
        }

        //cross attack
        for (int dir = 4; dir < 8; dir++) {
            int direction_offset = DIRECTION_OFFSETS[dir];
            int steps = edges[square][dir];
            for (int i = 0, cur_square = square; i < steps; i++) {
                cur_square += direction_offset;
                int piece = board[cur_square];
                if (piece != 0) {
                    if (get_color(piece) == attacker_color) {
                        int piece_type = get_type(piece);
                        if (piece_type == PieceType::BISHOP || piece_type == PieceType::QUEEN || 
                            (piece_type == PieceType::KING && i == 0)) {
                            return true;
                        }
                    }
                    break;
                }
            }
        }

        //pawn attack
        int start;
        if (attacker_color == Color::BLACK) {
            start = 4;
        } else {
            start = 6;
        }
        for (int dir = start; dir < start + 2; dir++) {
            int direction_offset = DIRECTION_OFFSETS[dir];
            if (edges[square][dir]) {
                int cur_square = square + direction_offset;
                int piece = board[cur_square];
                if (piece == (attacker_color | PieceType::PAWN)) {
                    return true;
                }
            }
        }

        //knight attack
        std::vector<int> knight_moves = generate_knight_squares(square);
        for (int sq : knight_moves) {
            if (board[sq] == (attacker_color | PieceType::KNIGHT)) {
                return true;
            }
        }

        return false;
    }

    std::vector<int> generate_knight_squares(int square) {
        int r = get_rank(square);
        int f = get_file(square);
        int dest = square + SOUTH + SOUTH + EAST;
        std::vector<int> moves;
        if(r > 1 && f < 7) {
            moves.push_back(dest);
        }
        dest = square + NORTH + NORTH + EAST;
        if(r < 6 && f < 7) {
            moves.push_back(dest);
        }
        dest = square + NORTH + NORTH + WEST;
        if(r < 6 && f > 0) {
            moves.push_back(dest);
        }
        dest = square + SOUTH + SOUTH + WEST;
        if(r > 1 && f > 0) {
            moves.push_back(dest);
        }
        dest = square + SOUTH + EAST + EAST;
        if(r > 0 && f < 6) {
            moves.push_back(dest);
        }
        dest = square + NORTH + EAST + EAST;
        if(r < 7 && f < 6) {
            moves.push_back(dest);
        }
        dest = square + NORTH + WEST + WEST;
        if(r < 7 && f > 1) {
            moves.push_back(dest);
        }
        dest = square + SOUTH + WEST + WEST;
        if(r > 0 && f > 1) {
            moves.push_back(dest);
        }
        return moves;
    }

    std::vector<Move> generate_straight_moves(int square, int color, int steps_limit = 8) {
        std::vector<Move> moves;
        for (int dir = 0; dir < 4; dir++) {
            int direction_offset = DIRECTION_OFFSETS[dir];
            int steps = edges[square][dir];
            for (int i = 0, cur_square = square; i < std::min(steps, steps_limit); i++) {
                Move move(square, cur_square, cur_square, board[square], board[cur_square], board[cur_square], MoveType::NORMAL);
                cur_square += direction_offset;
                int piece = board[cur_square];
                if (piece != 0) {
                    if (get_color(piece) != color) {
                        moves.push_back(move);
                    }
                    break;
                }
                moves.push_back(move);
            }
        }
        return moves;
    }

    std::vector<Move> generate_cross_moves(int square, int color, int steps_limit = 8) {
        std::vector<Move> moves;
        for (int dir = 4; dir < 8; dir++) {
            int direction_offset = DIRECTION_OFFSETS[dir];
            int steps = edges[square][dir];
            for (int i = 0, cur_square = square; i < std::min(steps, steps_limit); i++) {
                Move move(square, cur_square, cur_square, board[square], board[cur_square], board[cur_square], MoveType::NORMAL);
                cur_square += direction_offset;
                int piece = board[cur_square];
                if (piece != 0) {
                    if (get_color(piece) != color) {
                        moves.push_back(move);
                    }
                    break;
                }
                moves.push_back(move);
            }
        }
        return moves;
    }

    std::vector<Move> generate_pawn_moves(int square, int color) {
        std::vector<Move> moves;
        std::vector<int> dirs;
        int opp_dir = NORTH;
        if (color == Color::WHITE) {
            opp_dir = SOUTH;
            dirs = {0, 4, 5};
        } else {
            dirs = {3, 6, 7};
        }
        for (int i = 1; i < 3; i++) {
            int direction_offset = DIRECTION_OFFSETS[dirs[i]];
            int steps = edges[square][dirs[i]];
            if (steps == 0) {
                continue;
            }
            int cur_square = square + direction_offset;
            int piece = board[cur_square];
            if (get_color(piece) == color) {
                continue;
            }
            if (piece) {
                int rank = get_rank(cur_square);
                if (rank == 7 || rank == 0) {
                    for (int type : PROMOTION_TYPES) {
                        Move move(square, cur_square, cur_square, board[square], color | type, board[cur_square], MoveType::PROMOTION);
                        moves.push_back(move);
                    }
                } else {
                    Move move(square, cur_square, cur_square, board[square], board[square], board[cur_square], MoveType::NORMAL);
                    moves.push_back(move);
                }
            } else if (en_passant_square == cur_square) {
                Move move(square, cur_square, cur_square + opp_dir, board[square], board[square], board[cur_square + opp_dir], MoveType::EN_PASSANT);
                moves.push_back(move);
            }
        }
        int steps = edges[square][dirs[0]];
        int direction_offset = DIRECTION_OFFSETS[dirs[0]];
        int cur_square = square + direction_offset;
        if (steps >= 1 && board[cur_square] == 0) {
            int rank = get_rank(cur_square);
            if (rank == 7 || rank == 0) {
                for (int type : PROMOTION_TYPES) {
                    Move move(square, cur_square, cur_square, board[square], color | type, board[cur_square], MoveType::PROMOTION);
                    moves.push_back(move);
                }
            } else {
                Move move(square, cur_square, cur_square, board[square], board[square], board[cur_square], MoveType::NORMAL);
                moves.push_back(move);
            }
        }
        int rank = get_rank(square);
        cur_square += direction_offset;
        if (((color == Color::WHITE && rank == 1) || (color == Color::BLACK && rank == 6)) && board[cur_square + direction_offset] == 0 && board[cur_square + 2 * direction_offset] == 0) {
            Move move(square, cur_square, cur_square, board[square], board[square], board[cur_square], MoveType::NORMAL);
            moves.push_back(move);
        } 
        return moves;
    }

    std::vector<Move> generate_knight_moves(int square, int color) {
        std::vector<Move> moves;
        for (int sq : generate_knight_squares(square)) {
            if (get_color(board[sq]) != color) {
                Move move(square, sq, sq, board[square], board[sq], board[sq], MoveType::NORMAL);
                moves.push_back(move);
            }
        }
        return moves;
    }

    std::vector<Move> generaet_king_moves(int square, int color) {
        std::vector<Move> moves;
        for (int dir = 0; dir < 8; dir++) {
            int direction_offset = DIRECTION_OFFSETS[dir];
            int steps = edges[square][dir];
            if (steps) {
                int cur_square = square + direction_offset;
                if (get_color(board[cur_square]) != color) {
                    Move move(square, cur_square, cur_square, board[square], board[square], board[cur_square], MoveType::NORMAL);
                    moves.push_back(move);
                }
            }
        }

        int opp_color = Color::WHITE;
        if (color == Color::WHITE) {
            opp_color = Color::BLACK;
        }
        if ((square == 4 && castle_rights & 4) || (square == 60 && castle_rights & 1)) {
            if (castle_rights & 4 && board[square + EAST] == 0 && board[square + 2 * EAST] == 0
                && !is_square_attacked(square, opp_color) && !is_square_attacked(square + EAST, opp_color) 
                && !is_square_attacked(square + 2 * EAST, opp_color)) {
                Move move(square, square + 2 * EAST, square + 2 * EAST, board[square], board[square], 0, MoveType::KING_CASTLE);
                moves.push_back(move);
            }
        }
        if ((square == 4 && castle_rights & 8) || (square == 60 && castle_rights & 2)) {
            if (castle_rights & 4 && board[square + WEST] == 0 && board[square + 2 * WEST] == 0
                && !is_square_attacked(square, opp_color) && !is_square_attacked(square + WEST, opp_color) 
                && !is_square_attacked(square + 2 * WEST, opp_color)) {
                Move move(square, square + 2 * WEST, square + 2 * WEST, board[square], board[square], 0, MoveType::QUEEN_CASTLE);
                moves.push_back(move);
            }
        }
        return moves;
    }

    std::vector<Move> generate_pseudo_legal_moves(int color) {
        std::vector<Move> moves;
        for (int i = 0; i < 64; i++) {
            int piece = board[i];
            if (get_color(piece) == color) {
                int type = get_type(piece);
                std::vector<Move> cur_moves;
                if (type == PieceType::QUEEN || type == PieceType::ROOK) {
                    cur_moves = generate_straight_moves(i, color);
                    moves.insert(moves.end(), cur_moves.begin(), cur_moves.end());
                }
                if (type == PieceType::QUEEN || type == PieceType::BISHOP) {
                    cur_moves = generate_cross_moves(i, color);
                    moves.insert(moves.end(), cur_moves.begin(), cur_moves.end());
                }
                if (type == PieceType::KING) {
                    cur_moves = generate_straight_moves(i, color, 1);
                    moves.insert(moves.end(), cur_moves.begin(), cur_moves.end());
                    cur_moves = generate_cross_moves(i, color, 1);
                    moves.insert(moves.end(), cur_moves.begin(), cur_moves.end());
                }
                if (type == PieceType::PAWN) {
                    cur_moves = generate_pawn_moves(i, color);
                    moves.insert(moves.end(), cur_moves.begin(), cur_moves.end());
                }
                if (type == PieceType::KNIGHT) {
                    cur_moves = generate_knight_moves(i, color);
                    moves.insert(moves.end(), cur_moves.begin(), cur_moves.end());
                }
                if (type == PieceType::KING) {
                    cur_moves = generaet_king_moves(i, color);
                    moves.insert(moves.end(), cur_moves.begin(), cur_moves.end());
                }
            }
        }
        return moves;
    }

    std::vector<Move> generate_moves(int color) {
        int king_square = white_king_square;
        int opp_color = Color::BLACK;
        if (color == Color::BLACK) {
            king_square = black_king_square;
            opp_color = Color::WHITE;
        }
        std::vector<Move> pseudo_moves = generate_pseudo_legal_moves(color);
        std::vector<Move> moves;
        for (Move &move : pseudo_moves) {
            make_move(move);
            if (!is_square_attacked(king_square, opp_color)) {
                moves.push_back(move);
            }
            unmake_move();
        }
        return moves;
    }
};

int main() {
    std::vector<std::string> fens = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
        "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2",
        "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
    };
    Game game(fens[0]);
    std::vector<Move> moves = game.generate_moves(Color::WHITE);
    std::cout << moves.size() << "\n";
    return 0;
}