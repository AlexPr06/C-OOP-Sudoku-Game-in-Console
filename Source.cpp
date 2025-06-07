#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <random>
#include <algorithm>
#include <ctime>
#include <numeric>

using namespace std;

const int SIZE = 9;

class Sudoku {
private:
    vector<vector<int>> board;

public:
    Sudoku() : board(SIZE, vector<int>(SIZE, 0))
    {
        generateBoard();
    }

	

    void loadBoard(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Unable to open file.\n";
            return;
        }
        for (int i = 0; i < SIZE; ++i)
            for (int j = 0; j < SIZE; ++j)
                file >> board[i][j];
        file.close();
    }

    void saveBoard(const string& filename) {
        ofstream file(filename);
        if (!file) {
            cerr << "Unable to save file.\n";
            return;
        }
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                file << board[i][j] << " ";
            }
            file << "\n";
        }
        file.close();
    }

    void printBoard() {
        cout << "\n    ";
        for (int col = 0; col < SIZE; ++col) cout << col << " ";
        cout << "\n   -------------------\n";
        for (int i = 0; i < SIZE; ++i) {
            cout << i << " | ";
            for (int j = 0; j < SIZE; ++j) {
                if (board[i][j] == 0)
                    cout << ". ";
                else
                    cout << board[i][j] << " ";
            }
            cout << "\n";
        }
    }

    bool isValid(int row, int col, int num) {
        for (int i = 0; i < SIZE; ++i) {
            if (board[row][i] == num || board[i][col] == num)
                return false;
        }

        int startRow = row - row % 3;
        int startCol = col - col % 3;

        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (board[startRow + i][startCol + j] == num)
                    return false;

        return true;
    }

    bool solve() {
        for (int row = 0; row < SIZE; ++row) {
            for (int col = 0; col < SIZE; ++col) {
                if (board[row][col] == 0) {
                    for (int num = 1; num <= SIZE; ++num) {
                        if (isValid(row, col, num)) {
                            board[row][col] = num;
                            if (solve()) return true;
                            board[row][col] = 0;
                        }
                    }
                    return false;
                }
            }
        }
        return true;
    }

    void setCell(int row, int col, int value) {
        if (row >= 0 && row < SIZE && col >= 0 && col < SIZE)
            board[row][col] = value;
    }

    int getCell(int row, int col) const {
        if (row >= 0 && row < SIZE && col >= 0 && col < SIZE)
            return board[row][col];
        return -1;
    }



	// Generate Sudoku puzzle
    void generateBoard(int emptyCells = 40) {
        board = vector<vector<int>>(SIZE, vector<int>(SIZE, 0));
        fillDiagonalBoxes();
        solve(); // Заповнюємо дошку повністю
        removeCells(emptyCells);
    }

    void fillDiagonalBoxes() {
        for (int i = 0; i < SIZE; i += 3) {
            fillBox(i, i);
        }
    }

    void fillBox(int row, int col) {
        vector<int> nums(SIZE);
        iota(nums.begin(), nums.end(), 1);
        shuffle(nums.begin(), nums.end(), default_random_engine(time(0)));

        int idx = 0;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                board[row + i][col + j] = nums[idx++];
    }

    void removeCells(int count) {
        int removed = 0;
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, SIZE - 1);

        while (removed < count) {
            int row = dis(gen);
            int col = dis(gen);
            if (board[row][col] != 0) {
                int backup = board[row][col];
                board[row][col] = 0;

                // Копія дошки для перевірки
                vector<vector<int>> temp = board;
                int solutions = 0;
                countSolutions(temp, solutions);
                if (solutions != 1) {
                    board[row][col] = backup; // відновлюємо
                }
                else {
                    removed++;
                }
            }
        }
    }

    void countSolutions(vector<vector<int>>& b, int& count) {
        for (int row = 0; row < SIZE; ++row) {
            for (int col = 0; col < SIZE; ++col) {
                if (b[row][col] == 0) {
                    for (int num = 1; num <= SIZE; ++num) {
                        if (isValidCell(b, row, col, num)) {
                            b[row][col] = num;
                            countSolutions(b, count);
                            b[row][col] = 0;
                        }
                    }
                    return;
                }
            }
        }
        count++;
    }

    bool isValidCell(vector<vector<int>>& b, int row, int col, int num) {
        for (int i = 0; i < SIZE; ++i)
            if (b[row][i] == num || b[i][col] == num)
                return false;

        int startRow = row - row % 3;
        int startCol = col - col % 3;

        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (b[startRow + i][startCol + j] == num)
                    return false;

        return true;
    }

};

class Game {
private:
    Sudoku sudoku;

public:
    void start() {
        int choice;
        while (true) {
            showMenu();
            cout << "Choose an option: ";
            cin >> choice;
            switch (choice) {
            case 1:
                sudoku.printBoard();
                break;
            case 2:
                userInput();
                break;
            case 3:
                if (sudoku.solve()) {
                    cout << "Solved!\n";
                    sudoku.printBoard();
                }
                else {
                    cout << "No solution exists.\n";
                }
                break;
            case 4:
            {
                string filename;
                cout << "Enter filename to load: ";
                cin >> filename;
                sudoku.loadBoard(filename);
            }
            break;
            case 5:
            {
                string filename;
                cout << "Enter filename to save: ";
                cin >> filename;
                sudoku.saveBoard(filename);
            }
            break;
            case 6:
            {
                int difficulty;
                cout << "Enter number of empty cells (e.g. 40 for medium): ";
                cin >> difficulty;
                sudoku.generateBoard(difficulty);
                sudoku.printBoard();
            }
            break;
            case 0:
                cout << "Goodbye!\n";
                return;
            default:
                cout << "Invalid option.\n";
            }
        }
    }

    void userInput() {
        int row, col, value;
        cout << "Enter row (0-8): ";
        cin >> row;
        cout << "Enter column (0-8): ";
        cin >> col;
        cout << "Enter value (1-9): ";
        cin >> value;

        if (value < 1 || value > 9) {
            cout << "Invalid number!\n";
            return;
        }

        if (sudoku.isValid(row, col, value)) {
            sudoku.setCell(row, col, value);
            cout << "Value set successfully.\n";
        }
        else {
            cout << "Invalid move!\n";
        }
    }

    void showMenu() {
        cout << "\n=== Sudoku Menu ===\n";
        cout << "1. Show board\n";
        cout << "2. Enter number\n";
        cout << "3. Solve puzzle\n";
        cout << "4. Load from file\n";
        cout << "5. Save to file\n";
		cout << "6. Generate new puzzle\n";
        cout << "0. Exit\n";
    }
};

int main() {
    Game game;
    game.start();
    return 0;
}
