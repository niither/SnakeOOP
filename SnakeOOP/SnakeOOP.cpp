#include "Header.h"
using namespace std;

// Class realizations
Snake::Snake(int startX, int startY, int startLength)
    : direction(RIGHT), headSymbol('O'), bodySymbol('o'), color(LIGHTGREEN) {
    for (int i = 0; i < startLength; i++) {
        this->body.push_back(Point(startX - i, startY));
    }
}

void Snake::move() {
    for (int i = this->body.size() - 1; i > 0; i--) {
        this->body[i] = this->body[i - 1]; // Move body part to the previous
    }

    Point head = this->body[0]; // Head is always the first (0th) part of snake
    switch (this->direction) { // Change in head's (and snake's) direction
    case UP:
        head.setY(head.getY() - 1);
        break;
    case DOWN:
        head.setY(head.getY() + 1);
        break;
    case LEFT:
        head.setX(head.getX() - 1);
        break;
    case RIGHT:
        head.setX(head.getX() + 1);
        break;
    }
    this->body[0] = head;
}

void Snake::grow() {
    Point tail = this->body.back();
    this->body.push_back(tail); // Creates new body element at the back
}

void Snake::changeDirection(int newDir) {
    if ((newDir == UP && this->direction != DOWN) ||
        (newDir == DOWN && this->direction != UP) ||
        (newDir == LEFT && this->direction != RIGHT) ||
        (newDir == RIGHT && this->direction != LEFT)) {
        this->direction = newDir;
    }
}

bool Snake::checkSelfCollision() const {
    Point head = this->body[0];
    for (int i = 1; i < this->body.size(); i++) {
        if (head.isEqual(this->body[i])) {
            return true;
        }
    }
    return false;
}

bool Snake::checkCollision(const Point& point) const {
    for (const auto& segment : this->body) {
        if (segment.isEqual(point)) {
            return true;
        }
    }
    return false;
}

void Snake::draw(HANDLE h) const {
    for (int i = 0; i < this->body.size(); i++) {
        SetConsoleTextAttribute(h, this->color);
        cout << (i == 0 ? this->headSymbol : this->bodySymbol);
    }
}

GameMap::GameMap(int w, int h) : width(w), height(h) {
    this->initializeGrid();
}

void GameMap::initializeGrid() {
    this->grid.resize(this->height, vector<int>(this->width)); // Change grid size from 0 {} to game's size
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            if (i == 0 || j == 0 || i == this->height - 1 || j == this->width - 1) { // Place walls around the grid
                this->grid[i][j] = WALL;
            }
            else {
                this->grid[i][j] = HALL;
            }
        }
    }
}

void GameMap::addWall(int x, int y) {
    if (this->isFree(x, y)) {
        this->grid[y][x] = WALL;
    }
}

bool GameMap::isFree(int x, int y) const {
    return this->grid[y][x] == HALL;
}

void GameMap::draw(HANDLE h, const Snake& snake, const Food& food) const {
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            bool printed = false;
            Point current(x, y);

            if (current.isEqual(food.getPosition())) {
                food.draw(h);
                printed = true;
            }
            else {
                const vector<Point>& body = snake.getBody();
                for (int i = 0; i < body.size(); i++) {
                    if (body[i].isEqual(current)) {
                        SetConsoleTextAttribute(h, LIGHTGREEN);
                        cout << (i == 0 ? 'O' : 'o');
                        printed = true;
                        break;
                    }
                }
            }

            if (!printed) {
                if (this->grid[y][x] == WALL) {
                    Wall wall(x, y);
                    wall.draw(h);
                }
                else {
                    cout << ' ';
                }
            }
        }
        SetConsoleTextAttribute(h, WHITE);
        cout << '\n';
    }
}

void ScoreManager::saveScore(int score) {
    int currentScore = this->loadScore();
    if (score > currentScore) {
        ofstream file(this->filename);
        if (file.is_open()) {
            file << score;
            file.close();
        }
    }
}

int ScoreManager::loadScore() const {
    int bestScore = 0;
    ifstream file(this->filename);
    if (file.is_open()) {
        file >> bestScore;
        file.close();
    }
    return bestScore;
}

Console::Console() {
    this->handle = GetStdHandle(STD_OUTPUT_HANDLE);
}

void Console::setup() {
    this->hideCursor();
    system("mode con cols=100 lines=50");
    system("title SnakeOOP");
}

void Console::gotoXY(int x, int y) const {
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(this->handle, c);
}

void Console::setColor(Color color) const {
    SetConsoleTextAttribute(this->handle, color);
}

void Console::hideCursor() const {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(this->handle, &cursorInfo);
}

void Console::printTitle() const {
    SetConsoleTextAttribute(this->handle, GREEN);
    const char* title[] = {
        "    ########                                ###                  ",
        "  ###########                               ###                  ",
        " ###       #                                ##     ##       ###  ",
        " ##           ##   ##           #####       ##   ####      ##### ",
        "  #####      ### #####        ###  ##       ##  ###       ##  ## ",
        "    #######  ## ### ###      ##    ##     ### ###        ######  ",
        "         ### ####    ##   ####    ###  #########        #####    ",
        "         ### ###     #########  #########  ### ###    ### ##  ###",
        "###########  ##       ####  ######   ##    ##   #######   ###### ",
        " ########                     ##            #      ###     ####  "
    };

    int lines = sizeof(title) / sizeof(title[0]);
    for (int i = 0; i < lines; i++) {
        this->gotoXY(28, 1 + i);
        cout << title[i] << "\n";
    }
    SetConsoleTextAttribute(this->handle, WHITE);
    cout << "\n\n";
}

int MainMenu::show() {
    int choice;
    do {
        this->console->clear();
        this->console->printTitle();
        this->console->setColor(LIGHTPURPLE);
        cout << "\t\t\t\t\t\t======== MENU ========\n\n";
        this->console->setColor(WHITE);
        cout << "\t\t\t\t\t\t1. Easy\n";
        cout << "\t\t\t\t\t\t2. Hard\n";
        cout << "\t\t\t\t\t\t3. Exit\n\n";
        this->console->setColor(LIGHTPURPLE);
        cout << "\t\t\t\t\t\tBest score: " << this->bestScore << "\n";
        cout << "\t\t\t\t\t\tYour choice (1-3): ";
        this->console->setColor(WHITE);
        cin >> choice;
    } while (choice < 1 || choice > 3);
    return choice;
}

Game::Game() : console(nullptr), map(nullptr), snake(nullptr),
food(nullptr), scoreManager(nullptr), score(0),
difficulty(1), gameOver(false) {
    this->console = new Console();
    this->console->setup();
    this->scoreManager = new ScoreManager("bestScore.txt");
}

Game::~Game() {
    if (this->map != nullptr) {
        delete this->map;
        this->map = nullptr;
    }
    if (this->snake != nullptr) {
        delete this->snake;
        this->snake = nullptr;
    }
    if (this->food != nullptr) {
        delete this->food;
        this->food = nullptr;
    }
    if (this->scoreManager != nullptr) {
        delete this->scoreManager;
        this->scoreManager = nullptr;
    }
    if (this->console != nullptr) {
        delete this->console;
        this->console = nullptr;
    }
}

void Game::initialize(int diff) {
    this->difficulty = diff;
    this->score = 0;
    this->gameOver = false;

    if (this->map != nullptr) {
        delete this->map;
        this->map = nullptr;
    }
    if (this->snake != nullptr) {
        delete this->snake;
        this->snake = nullptr;
    }
    if (this->food != nullptr) {
        delete this->food;
        this->food = nullptr;
    }

    const int width = 40;
    const int height = 20;

    this->map = new GameMap(width, height);
    this->snake = new Snake(width / 2, height / 2, 3);

    this->generateFood();
}

void Game::generateFood() {
    srand(time(0));
    int x, y;
    do {
        x = rand() % (this->map->getWidth() - 2) + 1;
        y = rand() % (this->map->getHeight() - 2) + 1;
    } while (!this->map->isFree(x, y) || this->snake->checkCollision(Point(x, y)));

    if (this->food != nullptr) {
        delete this->food;
        this->food = nullptr;
    }
    this->food = new Food(x, y);
    this->map->setCell(x, y, FOOD);
}

void Game::generateWall() {
    int x, y;
    do {
        x = rand() % (this->map->getWidth() - 1);
        y = rand() % (this->map->getHeight() - 1);
    } while (!this->map->isFree(x, y) || this->snake->checkCollision(Point(x, y)));

    this->map->addWall(x, y);
}

void Game::handleInput() {
    if (_kbhit()) {
        int key = _getch();
        if (key == ESCAPE) {
            this->gameOver = true;
        }
        else {
            this->snake->changeDirection(key);
        }
    }
}

void Game::update() {
    this->snake->move();

    Point head = this->snake->getHeadPosition();

    if (this->map->getCell(head.getX(), head.getY()) == WALL) {
        this->gameOver = true;
    }

    if (this->snake->checkSelfCollision()) {
        this->gameOver = true;
    }

    if (head.isEqual(this->food->getPosition())) {
        this->snake->grow();
        this->score += 10;
        this->map->setCell(head.getX(), head.getY(), HALL);
        this->generateFood();

        if (this->difficulty == 2) {
            this->generateWall();
        }
    }
}

void Game::render() {
    this->console->gotoXY(0, 0);
    this->map->draw(this->console->getHandle(), *this->snake, *this->food);
    this->console->setColor(WHITE);
    cout << "Score: " << this->score << '\n';
}

bool Game::checkGameOver() {
    return this->gameOver;
}

int Game::run() {
    while (!this->checkGameOver()) {
        this->handleInput();
        this->update();
        this->render();
        Sleep(100);
    }

    this->scoreManager->saveScore(this->score);
    return this->score;
}

int main() {
    Game game;
    ScoreManager scoreManager("bestScore.txt");
    int bestScore = scoreManager.loadScore();

    while (true) {
        Console tempConsole;
        tempConsole.setup();
        MainMenu mainMenu(&tempConsole, bestScore);

        int choice = mainMenu.show();
        if (choice == 3) {
            break;
        }

        tempConsole.clear();
        game.initialize(choice);
        int score = game.run();

        if (score > bestScore) {
            scoreManager.saveScore(score);
            bestScore = score;
        }

        tempConsole.clear();
        cout << "Game Over! Final Score: " << score << "\nBest Score: " << bestScore
            << "\nPress any key to return to menu...";
        _getch();
    }

    return 0;
}