#pragma once

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