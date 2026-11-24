#include<algorithm>
#include<array>
#include<cctype>
#include<iostream>
#include<vector>
#include<string>
#include<random>
#include<utility>

//---Resources---

class Resources {
private:
    int steps;
    int keys;
    int gold;
    int diamonds;

public:
    explicit Resources(int steps = 35, int keys = 1, int gold = 0, int diamonds = 2);

    int getSteps() const;
    int getKeys() const;
    int getGold() const;
    int getDiamonds() const;

    bool useSteps(int amount = 1);
    void gainSteps(int amount);

    bool useDiamonds(int amount);
    void gainDiamonds(int amount);

    bool useKeys(int amount = 1);
    void gainKeys(int amount);

    void gainGold(int amount);

    friend std::ostream &operator<<(std::ostream &out, const Resources &res);
};

Resources::Resources(int steps, int keys, int gold, int diamonds) : steps(steps), keys(keys), gold(gold), diamonds(diamonds) {}

int Resources::getSteps() const { return steps; }
int Resources::getKeys() const { return keys; }
int Resources::getGold() const { return gold; }
int Resources::getDiamonds() const { return diamonds; }

bool Resources::useSteps(int amount) {
    if (steps < amount) {
        return false;
    }
    steps -= amount;
    return true;
}
void Resources::gainSteps(int amount) { steps += amount; }

bool Resources::useDiamonds(int amount) {
    if (diamonds < amount) {
        return false;
    }
    diamonds -= amount;
    return true;
}
void Resources::gainDiamonds(int amount) { diamonds += amount; }

bool Resources::useKeys(int amount) {
    if (keys <= amount) {
        return false;
    }
    keys -= amount;
    return true;
}
void Resources::gainKeys(int amount) { keys += amount; }
void Resources::gainGold(int amount) { gold += amount; }

std::ostream &operator<<(std::ostream &out, const Resources &res) {
    out << "Steps "<< res.steps << " Keys " << res.keys << " Gold " << res.gold << " Diamonds " << res.diamonds;
    return out;
}

//---Room---

enum class Direction {
    North = 0,
    East = 1,
    South = 2,
    West = 3
};

Direction oppDirection(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 2) % 4);
}

class Room {
private:
    std::string name;
    char symbol;
    int diamondPrice;
    std::array<bool, 4> doors;
    int bonusDiamonds;
    int bonusSteps;
    int bonusKeys;
    bool visited;
    bool antechamber;

public:
    Room(std::string name = "Empty",
        char symbol = ' ',
        int diamondPrice = 0,
        std::array<bool, 4> doors = {false, false, false, false},
        int bonusDiamonds = 0,
        int bonusSteps = 0,
        int bonusKeys = 0,
        bool antechamber = false);

    void rotate90();
    bool hasDoor(Direction dir) const;

    const std::string& getName() const;
    char getSymbol() const;
    int getDiamondPrice() const;
    bool isAntechamber() const;
    bool isVisited() const;

    void applyBonus(Resources& res);

    friend std::ostream& operator<< (std::ostream &out, const Room &room);

};

Room::Room(std::string name,
                char symbol,
                int diamondPrice,
                std::array<bool, 4> doors,
                int bonusDiamonds,
                int bonusSteps,
                int bonusKeys,
                bool antechamber)
        : name(std::move(name)),
        symbol(symbol),
        diamondPrice(diamondPrice),
        doors(doors),
        bonusDiamonds(bonusDiamonds),
        bonusSteps(bonusSteps),
        bonusKeys(bonusKeys),
        visited(false),
        antechamber(antechamber){}

void Room::rotate90() {
    bool temp = doors[3];
    doors[3] = doors[2];
    doors[2] = doors[1];
    doors[1] = doors[0];
    doors[0] = temp;
}

bool Room::hasDoor(Direction dir) const {
    return doors[static_cast<int>(dir)];
}

const std::string& Room::getName() const {return name;}
char Room::getSymbol() const {return symbol;}
int Room::getDiamondPrice() const {return diamondPrice;}
bool Room::isVisited() const {return visited;}
bool Room::isAntechamber() const {return antechamber;}

void Room::applyBonus(Resources& res) {
    if (visited) {
        return;
    }
    visited = true;
    if (bonusSteps > 0) res.gainSteps(bonusSteps);
    if (bonusKeys > 0) res.gainKeys(bonusKeys);
    if (bonusDiamonds > 0) res.gainDiamonds(bonusDiamonds);
}

std::ostream& operator << (std::ostream& out, const Room& room) {
    out << room.name << "(" << room.symbol << ")"
        << "Price: " << room.diamondPrice << "Diamonds"
        <<  "Doors: "
        << (room.hasDoor(Direction::North) ? 'N' : '-')
        << (room.hasDoor(Direction::East) ? "E" : "-")
        << (room.hasDoor(Direction::South) ? 'S' : '-')
        << (room.hasDoor(Direction::West) ? 'W' : '-');

    if (room.bonusSteps > 0 || room.bonusDiamonds > 0 || room.bonusKeys > 0) {
        out << "Bonus: +  " << room.bonusSteps << " Steps + " << room.bonusDiamonds << " Diamonds + " << room.bonusKeys << " Keys";
    }

    return out;
}

//---Character---

class Character {
private:
    std::string name;
    int line;
    int column;
    Resources res;

public:
    Character(std::string name = "George", int line = 6, int column = 2);

    int getLine() const;
    int getColumn() const;
    Resources& getResource();
    const Resources& getResource() const;

    void setPosition(int newLine, int newColumn);

    friend std::ostream& operator<<(std::ostream& out, const Character& mc);
};

Character::Character(std::string name, int line, int colum)
         : name(std::move(name)), line(line), column(colum), res() {}

int Character::getLine() const {return line;}
int Character::getColumn() const {return column;}

Resources& Character::getResource() {return res;}
const Resources& Character::getResource() const {return res;}

void Character::setPosition(int newLine, int newColumn) {
    line = newLine;
    column = newColumn;
}

std::ostream& operator<<(std::ostream& out, const Character& mc) {
    out << "Character: " << mc.name << " at position " << mc.line << ", " << mc.column << std::endl << mc.res;
    return out;
}

//---Mansion---

class Mansion {
private:
    int lines;
    int columns;
    Room** grid;
    bool** placed;
    Character player;
    std::vector<Room> roomDeck;
    std::mt19937 rng;

    void memoryAllocation();
    void memoryDeallocation();
    void initializeDeck();
    void setupStartAndFinish();

    bool isInBounds(int line, int column) const;
    bool canPlace(const Room& room, int line, int column, Direction moveDir) const;
    std::vector<Room>drawOptions(int count = 3);
    bool executeDraft(int targetLine, int targetColumn, Direction moveDir);
    bool move(Direction direction);

public:
    Mansion(int lines = 7, int columns = 5);

    Mansion(const Mansion& other);
    Mansion& operator=(Mansion other);
    ~Mansion();

    void run();

    friend std::ostream& operator<<(std::ostream& out, const Mansion& mansion);

};

void Mansion::memoryAllocation() {
    grid = new Room*[lines];
    placed = new bool*[lines];
    for (int i = 0; i < lines; ++i) {
        grid[i] = new Room[columns];
        placed[i] = new bool[columns]{false};
    }
}

void Mansion::memoryDeallocation() {
    if (grid != nullptr ) {
        for (int i = 0; i < lines; ++i) {
            delete[] grid[i];
            delete[] placed[i];
        }
        delete[] grid;
        delete[] placed;
        grid = nullptr;
        placed = nullptr;
    }
}

void Mansion::initializeDeck() {
    roomDeck = {
        Room("Long Corridor",   'C', 0, {true,  false, true,  false}, 0, 0, 0),
        Room("T-Intersection",  'T', 1, {true,  true,  true,  false}, 0, 0, 0),
        Room("Central Hall",    'H', 2, {true,  true,  true,  true }, 2, 0, 0),
        Room("Corner Turn",     'L', 0, {true,  true,  false, false}, 0, 0, 0),
        Room("Dining Room",     'S', 1, {false, true,  true,  true }, 8, 0, 0),
        Room("Inner Garden",    'G', 1, {true,  false, true,  false}, 3, 2, 0),
        Room("Key Room",        'K', 1, {false, false, true,  true }, 0, 0, 2),
        Room("Pantry",          'P', 0, {false, false, true,  false}, 5, 1, 0),
        Room("Study",           'B', 2, {true,  true,  true,  false}, 0, 2, 1),
        Room("Library",         'M', 1, {true,  false, true,  true }, 4, 1, 0)
    };
}

void Mansion::setupStartAndFinish() {
    int startLine = lines - 1;
    int startColumn = columns / 2;
    player = Character("George", startLine, startColumn);

    grid[startLine][startColumn] = Room("Entrance Hall", 'I', 0, {true, true, false, true});
    grid[startLine][startColumn].applyBonus(player.getResource());
    placed[startLine][startColumn] = true;

    int endLine = 0;
    int endColumn = columns / 2;
    grid[endLine][endColumn] = Room("Antechamber", 'A', 0, {false, true, true, true}, 0, 0, 0, true);
    placed[endLine][endColumn] = true;
}

Mansion::Mansion(int lines, int columns)
    :lines(lines),
    columns(columns),
    grid(nullptr),
    placed(nullptr),
    player("George", lines - 1, columns / 2),
    rng(std::random_device{}()) {
    memoryAllocation();
    initializeDeck();
    setupStartAndFinish();
}

Mansion::Mansion(const Mansion& other)
    :lines(other.lines),
    columns(other.columns),
    grid(nullptr),
    placed(nullptr),
    player(other.player),
    roomDeck(other.roomDeck),
    rng(other.rng) {
    memoryAllocation();
    for (int i = 0; i < lines; ++i) {
        for (int j = 0; j < columns; ++j) {
            grid[i][j] = other.grid[i][j];
            placed[i][j] = other.placed[i][j];
        }
    }
}

Mansion& Mansion::operator=(Mansion other) {
    std::swap(lines, other.lines);
    std::swap(columns, other.columns);
    std::swap(grid, other.grid);
    std::swap(placed, other.placed);
    std::swap(player, other.player);
    std::swap(roomDeck, other.roomDeck);
    std::swap(rng, other.rng);
    return* this;
}

Mansion::~Mansion() {
    memoryDeallocation();
}

bool Mansion::isInBounds(int line, int column) const {
    return line >= 0 && line < lines && column >= 0 && column < columns;
}

bool Mansion::canPlace(const Room& room, int line, int column, Direction moveDir) const {
    if (!room.hasDoor(oppDirection(moveDir))) {
        return false;
    }
    if (line == 0 && room.hasDoor(Direction::North)) return false;
    if (line == lines - 1 && room.hasDoor(Direction::South)) return false;
    if (column == 0 && room.hasDoor(Direction::West)) return false;
    if (column == columns - 1 && room.hasDoor(Direction::East)) return false;
    return true;
}

std::vector<Room>Mansion::drawOptions(int count) {
    std::vector<Room> deckCopy = roomDeck;
    std::shuffle(deckCopy.begin(), deckCopy.end(), rng);
    if (static_cast<int>(deckCopy.size()) > count) {
        deckCopy.resize(count);
    }
    return deckCopy;
}

bool Mansion::executeDraft(int targetLine, int targetColumn, Direction moveDir) {
    std::vector<Room> options = drawOptions(3);

    while (true) {
        std::cout << "\n--- CHOOSE A ROOM ---\n";
        for (size_t i = 0; i < options.size(); ++i) {
            bool valid = canPlace(options[i], targetLine, targetColumn, moveDir);
            std::cout << (i + 1) << ". " << options[i] << (valid ? " FITS " : " NEEDS ROTATION ") << std::endl;
        }
        std::cout << "Pick a room, 'r' to rotate options, '0' to cancel" << std::endl;

        char choice;
        if (!(std::cin >> choice)) {return false;}
        if (choice == '0') {return false;}

        if (std::tolower(static_cast<unsigned char>(choice)) == 'r') {
            for (auto& room : options) {
                room.rotate90();
            }
            continue;
        }
        if (choice >= '1' && choice <= '3') {
            size_t index = static_cast<size_t>(choice) - '1';
            Room selectedRoom = options[index];

            if (!canPlace(selectedRoom, targetLine, targetColumn, moveDir)) {
                std::cout << "Doors do not align or face outwards! Use 'r' to rotate. " << std::endl;
                continue;
            }
            if (!player.getResource().useDiamonds(selectedRoom.getDiamondPrice())) {
                std::cout << "Not enough diamonds for this room! " << std::endl;
                continue;
            }

            grid[targetLine][targetColumn] = selectedRoom;
            placed[targetLine][targetColumn] = true;
            return true;
        }
    }
}

bool Mansion::move(Direction dir) {
    int currentLine = player.getLine();
    int currentColumn = player.getColumn();

    if (!grid[currentLine][currentColumn].hasDoor(dir)) {
        std::cout << "Door is facing a wall! " << std::endl;
        return false;
    }
    int dirLine[] = { -1, 0, 1, 0 };
    int dirColumn[] = { 0, 1, 0, -1 };
    int newLine = currentLine + dirLine[static_cast<int>(dir)];
    int newColumn = currentColumn + dirColumn[static_cast<int>(dir)];

    if (!isInBounds(newLine, newColumn)) {
        std::cout << "You cannot exit the mansion! " << std::endl;
        return false;
    }
    if (!placed[newLine][newColumn]) {
        if (!executeDraft(newLine, newColumn, dir)) {
            return false;
        }
    } else if (!grid[newLine][newColumn].hasDoor(oppDirection(dir))) {
            std::cout << "The adjacent room has a wall on this side! " << std::endl;
            return false;
        }
    if (!player.getResource().useSteps(1)) {
        return false;
    }

    player.setPosition(newLine, newColumn);
    grid[newLine][newColumn].applyBonus(player.getResource());
    std::cout << "Entered: " <<  grid[newLine][newColumn] << std::endl;
    return true;
}

void Mansion::run() {
    const Mansion initialBlueprint(*this);

    std::cout << "Goal: Reach the Antechamber [A] starting from the Entrance Hall [I]!" << std::endl;

    char command;
    while (true) {
        std::cout << std::endl << *this << std::endl;
        std::cout << player << std::endl;

        int line = player.getLine();
        int column = player.getColumn();

        if (grid[line][column].isAntechamber()) {
            std::cout << "\nCONGRATULATIONS! You reached the Antechamber!" << std::endl;
            break;
        }
        if (player.getResource().getSteps() <= 0) {
            std::cout << "\nYou ran out of steps! The day has ended." << std::endl;
            break;
        }
        std::cout << "Comands: W/A/S/D = move, N = reset day, Q = quit" << std::endl;
        if (!(std::cin >> command)) {
            break;
        }

        command = static_cast<char>(std::tolower(static_cast<unsigned char>(command)));
        if (command == 'q') {
            std::cout << "Game over!" << std::endl;
            break;
        }
        if (command == 'n') {
            *this = initialBlueprint;
            std::cout << " The mansion has been reset for a new day!" << std::endl;
            continue;
        }
        switch (command) {
            case 'w' : move(Direction::North); break;
            case 'd' : move(Direction::East); break;
            case 's' : move(Direction::South); break;
            case 'a' : move(Direction::West); break;
            default: std::cout << "Unknown command!" << std::endl; break;
        }
    }
}

std::ostream& operator<< (std::ostream& out, const Mansion& mansion) {
    for (int i = 0; i < mansion.lines; ++i) {
        for (int j = 0; j < mansion.columns; ++j) {
            if (!mansion.placed[i][j]) {
                out << " . ";
            } else {
                out << (mansion.grid[i][j].hasDoor(Direction::North) ? "+- -+" : "+---+");
            }
        }
        out << std::endl;

        for (int j = 0; j < mansion.columns; ++j) {
            if (!mansion.placed[i][j]) {
                std::cout << " . ";
            } else {
                char left = mansion.grid[i][j].hasDoor(Direction::West) ? ' ' : '|';
                char right = mansion.grid[i][j].hasDoor(Direction::East) ? ' ' : '|';
                char center = (i == mansion.player.getLine() && j == mansion.player.getColumn()) ? '@' : mansion.grid[i][j].getSymbol();
                out << left << " " << center << " " << right;
            }
        }
        out <<std::endl;

        for (int j = 0; j < mansion.columns; ++j) {
            if (!mansion.placed[i][j]) {
                out << " . ";
            } else {
                out << (mansion.grid[i][j].hasDoor(Direction::South) ? "+- -+" : "+---+");
            }
        }
        out << std::endl;
    }
    return out;
}

int main() {

    Mansion mansion(7, 5);
    mansion.run();

    return 0;
}
