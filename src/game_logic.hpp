#pragma once
// Гарантирует, что этот заголовочный файл подключается только один раз при компиляции
// (альтернатива #ifndef/#define/#endif)
#include <vector>   // подключаем вектор

struct Cell { // одна клетка
    bool has_mine = false;   // есть ли мина
    bool revealed = false;  // открыта ли игроком
    bool flagged = false;       // стоит ли флаг
    int neighbor_mines = 0;  // сколько мин рядом

};

// Эти глобальные константы определяется в .cpp-файле и используется во всех модулях
extern const int FIELD_SIZE;    //  размер поля
extern const int MINES;          // кол-во мин

// Упрощённое имя типа: Field
using Field = std::vector<std::vector<Cell>>;

// ===== Описание функций игрового движка =====
void place_mines(Field& field, int total_mines);
void count_neighbor_mines(Field& field);
bool show_cell(Field& field, int x, int y, int& quantity_of_safe_cell);
void initialize_game(Field& field, int& quantity_of_safe_cell, bool& game_over, bool& victory);
