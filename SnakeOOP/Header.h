#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>

using namespace std;

enum Color : short {
    BLACK, BLUE, GREEN, AQUA, RED, PURPLE, YELLOW, WHITE, GRAY, LIGHTBLUE,
    LIGHTGREEN, LIGHTAQUA, LIGHTRED, LIGHTPURPLE, LIGHTYELLOW, BRIGHTWHITE
};

enum Key : short {
    UP = 72, DOWN = 80, LEFT = 75, RIGHT = 77, ESCAPE = 27
};

enum GameObj : short {
    HALL, WALL, FOOD
};

// Coordinate class
class Point {
    int x;
    int y;
public:
    Point() : x(0), y(0) {}
    Point(int xVal, int yVal) : x(xVal), y(yVal) {}

    int getX() const { return this->x; }
    int getY() const { return this->y; }
    void setX(int xVal) { this->x = xVal; }
    void setY(int yVal) { this->y = yVal; }

	bool isEqual(const Point& other) const { // Compare two points
        return this->x == other.x && this->y == other.y;
    }
};

// Basic class for game objects
class GameObject {
protected:
    Point position;
    char symbol;
    Color color;
public:
    GameObject() : symbol(' '), color(WHITE) {}
    GameObject(int x, int y, char symbol, Color col)
        : position(x, y), symbol(symbol), color(col) {
    }
    virtual ~GameObject() {}

    virtual void draw(HANDLE h) const {
        SetConsoleTextAttribute(h, this->color);
        cout << this->symbol;
    }

    Point getPosition() const {
        return this->position;
    }
    void setPosition(int x, int y) {
        this->position.setX(x);
        this->position.setY(y);
    }
    char getSymbol() const { 
        return this->symbol; 
    }
    Color getColor() const { 
        return this->color; 
    }
};

class Food : public GameObject {
public:
    Food() : GameObject(0, 0, '*', LIGHTRED) {}
    Food(int x, int y) : GameObject(x, y, '*', LIGHTRED) {}

    void draw(HANDLE h) const override {
        GameObject::draw(h);
    }
};

class Wall : public GameObject {
public:
    Wall() : GameObject(0, 0, '#', BLUE) {}
    Wall(int x, int y) : GameObject(x, y, '#', BLUE) {}
};

// Snake class (player)
class Snake {
private:
    vector<Point> body;
    int direction;
    char headSymbol;
    char bodySymbol;
    Color color;
public:
    Snake(int startX, int startY, int startLength);
    ~Snake() {
        this->body.clear();
    }
    void move();
    void grow();
    void changeDirection(int newDir);
    bool checkSelfCollision() const;
    bool checkCollision(const Point& point) const;

    Point getHeadPosition() const {
        return this->body[0];
    }
    int getLength() const {
        return this->body.size();
    }
    int getDirection() const {
        return this->direction;
    }
    const vector<Point>& getBody() const {
        return this->body;
    }

    void draw(HANDLE h) const;
};

class GameMap {
private:
    int width;
    int height;
    int** grid;
    vector<Wall> walls;

    void initializeGrid();
    void deleteGrid();
public:
    GameMap(int w, int h);
    ~GameMap();

    int getWidth() const {
        return this->width;
    }
    int getHeight() const {
        return this->height;
    }
    int getCell(int x, int y) const {
        return this->grid[y][x];
    }
    void setCell(int x, int y, int value) {
        this->grid[y][x] = value;
    }

    void addWall(int x, int y);
    bool isFree(int x, int y) const;
    void draw(HANDLE h, const Snake& snake, const Food& food) const;
};

// File manager for saving and loading score
class ScoreManager {
private:
    string filename;
public:
    ScoreManager(const string& file) : filename(file) {}
    ~ScoreManager() {}

    void saveScore(int score);
    int loadScore() const;
};

class Console {
private:
    HANDLE handle;
public:
    Console();
    ~Console() {}

    void setup();
    void clear() const {
        system("cls");
    }
    void gotoXY(int x, int y) const;
    void setColor(Color color) const;
    HANDLE getHandle() const {
        return this->handle;
    }
    void hideCursor() const;
    void printTitle() const;
};

class Menu {
protected:
    Console* console;
public:
    Menu(Console* con) : console(con) {}
    virtual ~Menu() {}
    virtual int show() = 0;
};

class MainMenu : public Menu {
private:
    int bestScore;
public:
    MainMenu(Console* con, int score) : Menu(con), bestScore(score) {}
    ~MainMenu() {}

    int show() override;
    void setBestScore(int score) { this->bestScore = score; }
};

class Game {
private:
    Console* console;
    GameMap* map;
    Snake* snake;
    Food* food;
    ScoreManager* scoreManager;
    int score;
    int difficulty;
    bool gameOver;

    void generateFood();
    void generateWall();
    void handleInput();
    void update();
    void render();
    bool checkGameOver();
public:
    Game();
    ~Game();

    void initialize(int diff);
    int run();
    int getScore() const {
        return this->score;
    }
};