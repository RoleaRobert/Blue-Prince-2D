#include<algorithm>
#include<array>
#include<cctype>
#include<iostream>
#include<vector>
#include<string>
#include<random>
#include<utility>
#include<cstddef>
#include<fstream>
#include<sstream>
#include<stdexcept>

//---Resources---

class Resources {
private:
    int steps;
    int keys;
    int gold;
    int diamonds;

public:
    explicit Resources(int steps = 0, int keys = 0, int gold = 0, int diamonds = 0);

    int getSteps() const;
    int getKeys() const;
    int getGold() const;
    int getDiamonds() const;

    bool useSteps(int amount = 1);
    void gainSteps(int amount);

    bool useDiamonds(int amount);
    void gainDiamonds(int amount);

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

void Resources::gainKeys(int amount) { keys += amount; }
void Resources::gainGold(int amount) { gold += amount; }

std::ostream &operator<<(std::ostream &out, const Resources &res) {
    out << "Steps "<< res.getSteps() << " Keys " << res.getKeys() << " Gold " << res.getGold() << " Diamonds " << res.getDiamonds();
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
    explicit Room(std::string name = "Empty",
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
    return doors[static_cast<std::size_t>(dir)];
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
    out << room.getName() << "(" << room.symbol << ")"
        << "Price: " << room.diamondPrice << "Diamonds"
        <<  "Doors: "
        << (room.hasDoor(Direction::North) ? 'N' : '-')
        << (room.hasDoor(Direction::East) ? "E" : "-")
        << (room.hasDoor(Direction::South) ? 'S' : '-')
        << (room.hasDoor(Direction::West) ? 'W' : '-');

    if (room.bonusSteps > 0 || room.bonusDiamonds > 0 || room.bonusKeys > 0) {
        out << "Bonus: +  " << room.bonusSteps << " Steps + " << room.bonusDiamonds << " Diamonds + " << room.bonusKeys << " Keys";
    }
    if (room.isVisited()) {
        out << "Visited";
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
    explicit Character(std::string name = "", int line = 0, int column = 0, const Resources& res = Resources());

    int getLine() const;
    int getColumn() const;
    Resources& getResource();
    const Resources& getResource() const;

    void setPosition(int newLine, int newColumn);

    friend std::ostream& operator<<(std::ostream& out, const Character& mc);
};

Character::Character(std::string name, int line, int colum, const Resources& res)
         : name(std::move(name)), line(line), column(colum), res(res) {}

int Character::getLine() const {return line;}
int Character::getColumn() const {return column;}

Resources& Character::getResource() {return res;}
const Resources& Character::getResource() const {return res;}

void Character::setPosition(int newLine, int newColumn) {
    line = newLine;
    column = newColumn;
}

std::ostream& operator<<(std::ostream& out, const Character& mc) {
    out << "Character: " << mc.name << " at position " << mc.line << ", " << mc.column << std::endl << mc.getResource();
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
    Room startRoom;
    Room finishRoom;
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
    Mansion();

    Mansion(const Mansion& other);
    Mansion& operator=(Mansion other);
    ~Mansion();

    void run();

    friend std::ostream& operator<<(std::ostream& out, const Mansion& mansion);

};

void Mansion::memoryAllocation() {
    grid = new Room*[static_cast<std::size_t>(lines)];
    placed = new bool*[static_cast<std::size_t>(lines)];
    for (int i = 0; i < lines; ++i) {
        grid[i] = new Room[static_cast<std::size_t>(columns)];
        placed[i] = new bool[static_cast<std::size_t>(columns)]{false};
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
    std::ifstream file("assets/rooms.txt");
    if (!file.is_open()) {
        throw std::runtime_error("Could not open rooms.txt");
    }

    roomDeck.clear();
    bool hasSize = false;
    bool hasPlayer = false;
    bool hasStart = false;
    bool hasFinish = false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string type;

        if (!(iss >> type)) {
            continue;
        }
        if (type == "SIZE") {
            int fileLines = 0;
            int fileColumn = 0;
            if (iss >> fileLines >> fileColumn && fileLines >= 3 && fileColumn >= 3) {
                lines = fileLines;
                columns = fileColumn;
                hasSize = true;
            }
            continue;
        }
        if (type == "PLAYER") {
            int playerSteps = 0;
            int playerKeys = 0;
            int playerGold = 0;
            int playerDiamonds = 0;
            if (iss >> playerSteps >> playerKeys >> playerGold >> playerDiamonds ) {
                std::string playerName;
                std::getline(iss >> std::ws, playerName);
                if (!playerName.empty()) {
                    player = Character(playerName, 0, 0, Resources(playerSteps, playerKeys, playerGold, playerDiamonds));
                    hasPlayer = true;
                }
            }
            continue;
        }

        char symbol = ' ';
        int price = 0;
        int dirN = 0, dirE = 0, dirS = 0, dirW = 0;
        int bonusDiamonds = 0, bonusSteps = 0, bonusKeys = 0;

        if (!(iss >> symbol >> price >> dirN >> dirE >> dirS >> dirW >> bonusDiamonds >> bonusSteps >> bonusKeys)) {
            continue;
        }

        std::string roomName;
        std::getline(iss >> std::ws, roomName);
        if (roomName.empty()) {
            continue;
        }

        const std::array<bool, 4> doors = {dirN != 0, dirE != 0, dirS != 0, dirW != 0};
        if (type == "START") {
            startRoom = Room(roomName, symbol, price, doors, bonusDiamonds, bonusSteps, bonusKeys, false);
            hasStart = true;
        }else if (type == "FINISH") {
            finishRoom = Room(roomName, symbol, price, doors, bonusDiamonds, bonusSteps, bonusKeys, true);
            hasFinish = true;
        }else if (type == "DECK") {
            roomDeck.emplace_back(roomName, symbol, price, doors, bonusDiamonds, bonusSteps, bonusKeys, false);
        }
    }

    if (!hasSize || !hasPlayer || !hasStart || !hasFinish || roomDeck.empty()) {
        throw std::runtime_error("File incomplete");
    }
}

void Mansion::setupStartAndFinish() {
    const int startLine = lines - 1;
    const int startColumn = columns / 2;
    player.setPosition(startLine, startColumn);

    grid[startLine][startColumn] = startRoom;
    grid[startLine][startColumn].applyBonus(player.getResource());
    placed[startLine][startColumn] = true;

    int endLine = 0;
    int endColumn = columns / 2;
    grid[endLine][endColumn] = finishRoom;
    placed[endLine][endColumn] = true;
}

Mansion::Mansion()
    :lines(0),
    columns(0),
    grid(nullptr),
    placed(nullptr),
    player(),
    rng(std::random_device{}()) {
    initializeDeck();
    memoryAllocation();
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
    if (count > 0 && static_cast<std::size_t>(count) < deckCopy.size()) {
        deckCopy.resize(static_cast<std::size_t>(count));
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
            const std::size_t index = static_cast<std::size_t>(choice - '1');
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
    const int dirLine[] = { -1, 0, 1, 0 };
    const int dirColumn[] = { 0, 1, 0, -1 };
    const int newLine = currentLine + dirLine[static_cast<std::size_t>(dir)];
    const int newColumn = currentColumn + dirColumn[static_cast<std::size_t>(dir)];

    if (!isInBounds(newLine, newColumn)) {
        std::cout << "You cannot exit the mansion! " << std::endl;
        return false;
    }
    if (!placed[newLine][newColumn]) {
        if (!executeDraft(newLine, newColumn, dir)) {
            return false;
        }
        player.getResource().gainGold(1);
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
                out << "  .  ";
            } else {
                out << (mansion.grid[i][j].hasDoor(Direction::North) ? "+- -+" : "+---+");
            }
        }
        out << std::endl;

        for (int j = 0; j < mansion.columns; ++j) {
            if (!mansion.placed[i][j]) {
                out << "  .  ";
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
                out << "  .  ";
            } else {
                out << (mansion.grid[i][j].hasDoor(Direction::South) ? "+- -+" : "+---+");
            }
        }
        out << std::endl;
    }
    return out;
}

int main() {

    Mansion mansion;
    mansion.run();

    return 0;
}
