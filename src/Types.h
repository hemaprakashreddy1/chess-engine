#pragma once
#include <vector>

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