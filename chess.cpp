#include <iostream>
#include <vector>
#include <sstream>

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

struct StateInfo {
    int castle_rights;
    int en_passant_square;
};

struct Move {
    int from;
    int to;
    int capture_position;
    int from_piece;
    int to_piece;
    int captured_piece;
    MoveType move_type;
};

struct Game {
    int board[64]{};
    int turn;
    int half_move_cnt;
    int full_move_cnt;
    int en_passant_square;
    int castle_rights;
    std::vector<Move> moves;
    std::vector<StateInfo> states;

    Game(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
        load_game_from_fen(fen);
    }

    void load_game_from_fen(const std::string &fen) {
        std::stringstream ss(fen);
        int rank = 7, file = 0;
        // arranging pieces
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
                board[rank * 8 + file] = Color::BLACK | pieces[c - 'a'];
                file++;
            } else if (c >= 'A' && c <= 'Z') {
                board[rank * 8 + file] = Color::WHITE | pieces[c - 'A'];
                file++;
            } else if (c >= '1' && c <= '9') {
                file += c - '0';
            } else if (c == '/') {
                rank--;
                file = 0;
            }
        }

        //playres turn
        ss >> token;
        if (token == "w") {
            turn = Color::WHITE;
        } else {
            turn = Color::BLACK;
        }

        //castling rights
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

        //enpassant position
        ss >> token;
        en_passant_square = -1;
        if (token != "-") {
            en_passant_square = (token[1] - '0' - 1) * 8 + token[0] - 'a';
        }

        //halfmoves
        ss >> half_move_cnt;
        //full moves
        ss >> full_move_cnt;
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
                int color = piece & Color::COLOR_MASK;
                int piece_type = piece & PieceType::TYPE_MASK;
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
    }

};

int main() {
    std::vector<std::string> fens = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
        "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2",
        "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
    };

    for (auto fen : fens) {
        Game game(fen);
        game.print();
    }
    return 0;
}