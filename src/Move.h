#pragma once

#include "Types.h"
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