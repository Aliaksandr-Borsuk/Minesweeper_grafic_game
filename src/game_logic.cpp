#include "game_logic.hpp"
#include <cstdlib>

//const int FIELD_SIZE = 10;


void place_mines(Field& field, int mines) {
    /* функция установки мин
    создаём случайные координаты и ставит туда мины
    пока оне не закончатся
    */
    int placed = 0;
    while (placed < mines) {
        int x = rand() % FIELD_SIZE;
        int y = rand() % FIELD_SIZE;
        if (!field[y][x].has_mine) {
            field[y][x].has_mine = true;
            placed++;
        }
    }
}

void count_neighbor_mines(Field& field) {
    // считаем мины в соседних клетках
    for (int y = 0; y < FIELD_SIZE; ++y) {
        for (int x = 0; x < FIELD_SIZE; ++x) {
            if (field[y][x].has_mine) continue;  // если в клетке мина считать не будем
            int count = 0;
            // перебираем квадрат 3*3
            for (int dy = -1; dy <= 1; ++dy)
                for (int dx = -1; dx <= 1; ++dx){
                    // можно исключать из пересчёта центральную клетку проверяя if (dy || dx)
                    // но,кмк,  проще сложить лишний ноль чем 9 проверок
                    int ny = y + dy, nx = x + dx;
                    if (ny >= 0 && ny < FIELD_SIZE && nx >= 0 && nx < FIELD_SIZE && field[ny][nx].has_mine)
                        count++;
                }
            field[y][x].neighbor_mines = count;
        }
    }
}

bool show_cell(Field& field, int x, int y, int& quantity_of_safe_cell) {
    /*ф-я открывает клетку на позиции x, y
    если в клетке  и рядом с ней нет мин она рекурсивно открывает соседние клнтки
    возвращает false если рвануло или true если всё норм*/

    // если вышли за границы, или клетка уже открыта то заканчиваем
    if (x < 0 || x >= FIELD_SIZE || y < 0 || y >= FIELD_SIZE || field[y][x].revealed)
        return true;
    // меняем состояние клетки на открытая
    field[y][x].revealed = true;
    // уменьшаем счётчик закрытых безопасных клеток
    quantity_of_safe_cell--;
    // если нашли мину - проиграли
    if (field[y][x].has_mine) return false;
    // если мин рядом нет осматриваем соседей рекурсивно
    if (field[y][x].neighbor_mines == 0) {
        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx)
                if (dx || dy)    // саму клетку второй раз не смотрим хотя можно было бы ибо метка уже есть
                    show_cell(field, x + dx, y + dy, quantity_of_safe_cell);
    }
    // когда все соседи осмотрены - заканчиваем
    return true;
}

void initialize_game(Field& field, int& quantity_of_safe_cell, bool& game_over, bool& victory) {
    game_over = false;
    victory = false;
    quantity_of_safe_cell = FIELD_SIZE * FIELD_SIZE - MINES;
    field = Field(FIELD_SIZE, std::vector<Cell>(FIELD_SIZE));
    place_mines(field, MINES);      // расставляем мины
    count_neighbor_mines(field);    // считаем соседние мины
}
