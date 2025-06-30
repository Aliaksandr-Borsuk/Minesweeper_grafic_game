#include <SFML/Graphics.hpp>    // для графики
#include <vector>               // вектор
#include "game_logic.hpp"       // заголовочный файл с логикой

#include <iostream>             //
#include <windows.h>    // для корректного вывода кирилицы в консоли Windows

const int MINES = 10;
const int FIELD_SIZE = 10;  // размер игрового поля
const int TILE_SIZE = 48;   // размер одной клетки в пикселях


int main() {
    SetConsoleOutputCP(65001); // Устанавливаем UTF-8 для вывода в консоль на русском если нужно

    sf::Font font;
    if (!font.loadFromFile("../fonts/arial.ttf")) {
        // Если не получилось загрузить шрифт — выводим сообщение
        std::cerr << "Не удалось загрузить шрифт arial.ttf из папки fonts" << std::endl;
        return 1; // Завершаем программу с ошибкой
    }

    // графическое окно
    sf::RenderWindow window(
        sf::VideoMode(FIELD_SIZE * TILE_SIZE, FIELD_SIZE * TILE_SIZE),  // размер окна
        sf::String::fromUtf8("Сапёр", "Сапёр" + 10),                   // заголовок окна
        sf::Style::Titlebar | sf::Style::Close // Без возможности изменения размера
    );

    int quantity_of_safe_cell ;  //  счётчик закрытых безопасных клеток
    bool game_over ;     // флаг окончания игры
    bool victory;       // флаг победы
    Field field;    // игровое поле - двумерный вектор
    //  инициализация игры
    initialize_game(field, quantity_of_safe_cell, game_over, victory);

    // переменная для вывода финального сообщения
    sf::Text result_text;
    result_text.setFont(font);  // тот же шрифт, что и для цифр
    result_text.setCharacterSize(36);
    result_text.setFillColor(sf::Color::Red);
    result_text.setStyle(sf::Text::Bold);

    // создаём объект кнопки и финального текста
    sf::RectangleShape restart_button;
    restart_button.setSize(sf::Vector2f(200, 50));
    restart_button.setFillColor(sf::Color(100, 200, 250));
    restart_button.setPosition(FIELD_SIZE * TILE_SIZE / 2 - 100, FIELD_SIZE * TILE_SIZE / 2 + 40);

    sf::Text restart_text;
    restart_text.setFont(font);
    restart_text.setString(L"Играть ещё раз");
    restart_text.setCharacterSize(20);
    restart_text.setFillColor(sf::Color::Black);
    restart_text.setPosition(FIELD_SIZE * TILE_SIZE / 2 - 85, FIELD_SIZE * TILE_SIZE / 2 + 48);

    // Главный игровой цикл — работает, пока открыто окно
    while (window.isOpen()) {
        sf::Event event;

        // Обрабатываем все события окна (нажатие клавиш, мыши, закрытие)
        while (window.pollEvent(event)) {
            // Закрытие окна
            if (event.type == sf::Event::Closed)
                window.close();

            // Обработка кликов мыши
            if (event.type == sf::Event::MouseButtonPressed ) {
                // Получаем координаты точки, в которую кликнули
                int x = event.mouseButton.x;
                int y = event.mouseButton.y;

                // если включена кнопка играть ещё раз (game_over)
                // и мы в неё попали то перезапуск
                if (game_over && event.mouseButton.button == sf::Mouse::Left) {
                    if (restart_button.getGlobalBounds().contains(x, y)) {
                        // Перезапускаем игру
                        game_over = false;
                        victory = false;
                        quantity_of_safe_cell = FIELD_SIZE * FIELD_SIZE - MINES;

                        field = Field(FIELD_SIZE, std::vector<Cell>(FIELD_SIZE));
                        place_mines(field, MINES);
                        count_neighbor_mines(field);
                    }
                }
                // иначе обрабатываем как клик по игровому полю
                else{
                    // Получаем координаты клетки, в которую кликнули
                    x /= TILE_SIZE;
                    y /= TILE_SIZE;
                    // Проверяем, что координаты внутри границ поля
                    if (x >= 0 && x < FIELD_SIZE && y >= 0 && y < FIELD_SIZE) {
                        // левый клик
                        if (event.mouseButton.button == sf::Mouse::Left && !game_over){
                        // revealed[y][x] = true; // Открываем клетку
                            bool result = show_cell(field, x, y, quantity_of_safe_cell);
                            if (!result) {
                                game_over = true; // game over
                                // настраиваем финальное сообщение
                                result_text.setString(L"Ты подорвался!");
                                result_text.setFillColor(sf::Color(220, 20, 60));  // красный
                            }
                            else if(quantity_of_safe_cell == 0){
                                game_over = true; // game over
                                victory = true;   // победа
                                // настраиваем финальное сообщение
                                result_text.setString(L"Победа!");
                                result_text.setFillColor(sf::Color(0, 128, 0));    // зелёный
                            }
                        }
                        // правый клик
                        if (event.mouseButton.button == sf::Mouse::Right && !game_over){
                            if(!field[y][x].revealed){
                                // ставим флаг или убираем
                                field[y][x].flagged = !field[y][x].flagged;
                            }
                        }
                    }
                }
            }

        }

        // Очищаем экран белым фоном перед отрисовкой следующего кадра
        window.clear(sf::Color::White);

        // Отрисовываем каждую клетку поля
        for (int y = 0; y < FIELD_SIZE; ++y) {
            for (int x = 0; x < FIELD_SIZE; ++x) {
                // Создаём графический прямоугольник для клетки
                sf::RectangleShape cell(sf::Vector2f(TILE_SIZE - 2, TILE_SIZE - 2));  // уменьшаем на 2px для сетки
                cell.setPosition(x * TILE_SIZE + 1, y * TILE_SIZE + 1);  // устанавливаем позицию клетки с отступом

                // Определяем цвет клетки в зависимости от её состояния: открытая или закрытая
                if (field[y][x].revealed)
                    cell.setFillColor(sf::Color(200, 200, 200)); // открытая клетка светло-серый
                //////////////////////
                else if (field[y][x].flagged)
                    cell.setFillColor(sf::Color(255, 0, 0));      // флаг — красный
                else
                    cell.setFillColor(sf::Color(100, 100, 100)); // закрытая клетка тёмно-серый
                // Рисуем клетку в окне
                window.draw(cell);

                if (field[y][x].revealed) {    //} && field[y][x].neighbor_mines > 0) {
                    sf::Text number_text;
                    number_text.setFont(font);
                    number_text.setString(std::to_string(field[y][x].neighbor_mines));
                    number_text.setCharacterSize(24);  // размер текста
                    number_text.setFillColor(sf::Color::Black);
                    number_text.setPosition(x * TILE_SIZE + 16, y * TILE_SIZE + 8);  // небольшое смещение

                    window.draw(number_text);
                }
            }
        }
        // если игра окончена - финальное сообщение
        if (game_over) {
            sf::FloatRect text_bounds = result_text.getLocalBounds();
            result_text.setOrigin(text_bounds.width / 2, text_bounds.height / 2);
            result_text.setPosition(FIELD_SIZE * TILE_SIZE / 2, FIELD_SIZE * TILE_SIZE / 2);
            window.draw(result_text);

            window.draw(restart_button);
            window.draw(restart_text);
        }

        // Отображаем отрисованный кадр на экране
        window.display();
    }
    return 0;
}
