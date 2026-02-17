#include <iostream>
#include <vector>
#include <deque>
#include <Windows.h>
#include <random>
#include <conio.h>

enum Direction { UP, DOWN, LEFT, RIGHT };

const int FIELD_WIDTH = 25;
const int FIELD_HEIGHT = 20;
const char BORDER_CHAR = '#';
const char SPACE_CHAR = ' ';
const char APPLE_CHAR = '@';
const char SNAKE_CHAR = 'O';
const char UP_DIRECTION = 'w';
const char DOWN_DIRECTION = 's';
const char LEFT_DIRECTION = 'a';
const char RIGHT_DIRECTION = 'd';
const std::string GAME_OVER_MSG = "GAME OVER";

struct Game
{
    std::pair<int, int> apple;
    std::vector<std::vector<char>> field;
    std::deque<std::pair<int, int>> snake;
    Direction dir = RIGHT;
    bool isGameOver = false;
    int score = 0;
};


int GetRandomValue(int minValue, int maxValue)
{
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(minValue, maxValue);
    return dist(rng);
}

bool IsPositionIsSnake(const Game& game, int x, int y)
{
    for (const auto segment : game.snake)
        if (segment.first == x && segment.second == y)
            return true;
    return false;
}

bool IsOutOfField(const std::pair<int, int>& head)
{
    return head.first < 0 || head.first >= FIELD_WIDTH ||
           head.second < 0 || head.second >= FIELD_HEIGHT;
}

bool IsAppleEaten(const Game& game, const std::pair<int, int>& head)
{
    return head == game.apple;
}

Game GenerateSnake(const Game& game)
{
    Game newGame = game;
    int x = GetRandomValue(1, FIELD_WIDTH - 1);
    int y = GetRandomValue(1, FIELD_HEIGHT - 1);
    newGame.snake.clear();
    newGame.snake.push_front({ x, y });
    return newGame;
}
Game PlaceSnake(const Game& game)
{
    Game newGame = game;
    for (const auto s : newGame.snake)
        newGame.field[s.second][s.first] = SNAKE_CHAR;
    return newGame;
}

Game GenerateFood(const Game& game)
{
    Game newGame = game;
    int x, y;
    do
    {
        x = GetRandomValue(1, FIELD_WIDTH - 1);
        y = GetRandomValue(1, FIELD_HEIGHT - 1);
    }
    while (IsPositionIsSnake(newGame, x, y));

    newGame.apple = { x, y };
    return newGame;
}
Game PlaceFood(const Game& game)
{
    Game newGame = game;
    newGame.field[newGame.apple.second][newGame.apple.first] = APPLE_CHAR;
    return newGame;
}

Game UpdateField(const Game& game)
{
    Game newGame = game;
    newGame.field.assign(FIELD_HEIGHT, std::vector<char>(FIELD_WIDTH, SPACE_CHAR));
    newGame = PlaceSnake(newGame);
    newGame = PlaceFood(newGame);
    return newGame;
}

Game MoveSnake(const Game& game)
{
    Game newGame = game;
    auto head = newGame.snake.front();

    switch (newGame.dir)
    {
        case UP:    head.second--; break;
        case DOWN:  head.second++; break;
        case LEFT:  head.first--;  break;
        case RIGHT: head.first++;  break;
    }

    if (IsOutOfField(head) || IsPositionIsSnake(newGame, head.first, head.second))
    {
        newGame.isGameOver = true;
        return newGame;
    }

    if (IsAppleEaten(newGame, head))
    {
        newGame.snake.push_front(head);
        newGame.score += 1;
        newGame = GenerateFood(newGame);
        return newGame;
    }

    newGame.snake.push_front(head);
    newGame.snake.pop_back();
    return newGame;
}

Game Input(const Game& game)
{
    Game newGame = game;
    if (!_kbhit()) return game;
    switch (_getch())
    {
        case UP_DIRECTION:    if (newGame.dir != DOWN) newGame.dir = UP; break;
        case DOWN_DIRECTION:  if (newGame.dir != UP)   newGame.dir = DOWN; break;
        case LEFT_DIRECTION:  if (newGame.dir != RIGHT) newGame.dir = LEFT; break;
        case RIGHT_DIRECTION: if (newGame.dir != LEFT)  newGame.dir = RIGHT; break;
    }
    return newGame;
}

void ClearScreen()
{
    COORD coord = { 0, 0 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void PrintField(const Game& game)
{
    ClearScreen();
    for (int i = 0; i < FIELD_WIDTH + 2; i++) std::cout << BORDER_CHAR;
    std::cout << "\n";

    for (int i = 0; i < FIELD_HEIGHT; i++)
    {
        std::cout << BORDER_CHAR;
        for (int j = 0; j < FIELD_WIDTH; j++)
            std::cout << game.field[i][j];
        std::cout << BORDER_CHAR << "\n";
    }

    for (int i = 0; i < FIELD_WIDTH + 2; i++) std::cout << BORDER_CHAR;
    std::cout << "\n";
    std::cout << "Score: " << game.score << "\n";
}

Game InitGame()
{
    Game game;
    game.field.assign(FIELD_HEIGHT, std::vector<char>(FIELD_WIDTH, SPACE_CHAR));
    game.dir = RIGHT;
    game.isGameOver = false;
    game.score = 0;
    game = GenerateSnake(game);
    game = GenerateFood(game);
    game = UpdateField(game);
    return game;
}

int main()
{
    Game game = InitGame();
    Sleep(1000);
    while (!game.isGameOver)
    {
        game = Input(game);
        game = MoveSnake(game);
        game = UpdateField(game);
        PrintField(game);
        Sleep(100);
    }
    std::cout << GAME_OVER_MSG << std::endl;
    return 0;
}
