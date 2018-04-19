//============================================================================
// Name        : Minesweeper.cpp
// Author      : Luke Lewis
// Version     :
// Copyright   : 
// Description : Minesweeper
//============================================================================

#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <stdio.h>
#include <cstdlib>
#include <iomanip>
#include <stack>

using namespace std;
const int BEGINNER[2] = { 9, 10 }; //size, mines
const int INTERMEDIATE[2] = { 16, 40 };
const int ADVANCED[2] = { 24, 100 };

class Board {
public:
	Board(int level);
	vector<vector<char> > generateAdjacent(vector<vector<char> >);
	void displayReal();
	void display();
	void print(vector<vector<char> >);
	Board() {
		difficulty = 1;
		mines = BEGINNER[1];
		size = BEGINNER[0];
		tally = 0;
	}
	vector<vector<char> > realBoard;
	vector<vector<char> > adjacentBoard;
	vector<vector<char> > gameBoard;
	void play();
private:
	void mineCheck(int x, int y);
	void generate();
	char checkAdjacent(int, int);
	void clearArea(int x, int y);
	int difficulty;
	int mines;
	int size;
	int tally;
	stack<pair<int, int> > zeroes;
};

int main() {
	srand(time(0));
	int difficulty;
	cout << "Enter difficulty level between 1 and 3" << endl << "1 = BEGINNER"
			<< endl << "2 = INTERMEDIATE" << endl << "3 = ADVANCED" << endl;
	cin >> difficulty;
	Board board(difficulty);
	board.play();
}

Board::Board(int level) {
	difficulty = level;
	if (level == 1) {
		mines = BEGINNER[1];
		size = BEGINNER[0];
		tally = 0;
		generate();
	}
	if (level == 2) {
		mines = INTERMEDIATE[1];
		size = INTERMEDIATE[0];
		tally = 0;
		generate();
	}
	if (level == 3) {
		mines = ADVANCED[1];
		size = ADVANCED[0];
		tally = 0;
		generate();
	}
}

void Board::play() {
	bool exit = false;
	gameBoard.resize(size + 2, vector<char>(size + 2, '.')); //making gameBoard with default values set as '.'
	for (int i = 1; i <= size; i++) { //adding coordinate helpers around edges
		gameBoard[i][0] = (char) (i + '0');
		gameBoard[0][i] = (char) (i + '0');
		gameBoard[i][size + 1] = (char) (i + '0');
		gameBoard[size + 1][i] = (char) (i + '0');
	}
	int x = 0;
	int y = 0;
	while (!exit) {
		cout<<"Tally : "<<tally<<endl;
		if(tally == (size*size)-mines){//all non mines have been revealed, user wins. Exit loop/game
			exit = true;
			cout<<"Congratulations! You win!"<<endl;
			break;
		}
		cout<<endl;
		if (realBoard[x][y] == 'T') { //user selects a mine, loses. Exit loop/game
			gameBoard[x][y] = '*';
			exit = true;
			print(gameBoard);
			cout << "You have selected a mine and blew up" << endl;
			break;
		}
		print(gameBoard);
		cout << "enter row (between 1 and " << size << ")" << endl;
		cin >> x;
		cout << "enter column (between 1 and " << size << ")" << endl;
		cin >> y;
		mineCheck(x, y);
		while(!zeroes.empty()){ //using a stack to push on coordinates around selected zero that also has zeroes, so that we can call clearArea on those zeroes as well
			pair<int, int> toClear = zeroes.top();
			zeroes.pop();
			clearArea(toClear.first, toClear.second);
		}
	}
}

void Board::mineCheck(int x, int y) {
	if (adjacentBoard[x][y] == '0') {//clear surrounding blocks since they must not contain a mine
		clearArea(x, y);
	}
	if(gameBoard[x][y] == '.'){
		gameBoard[x][y] = adjacentBoard[x][y];
		tally++;
	}
}

void Board::clearArea(int x, int y) {
	for (int i = x - 1; i <= x + 1; i++) { //to loop over all locations surrounding the zero
		for (int j = y - 1; j <= y + 1; j++) {
			if (i > 0 && i <= size && j > 0 && j <= size) {
				if (adjacentBoard[i][j] == '0' && gameBoard[i][j] == '.') { //keeping track of zeroes around the zero, so that we can call clearArea on those automatically
					pair<int, int> toPush;
					toPush = make_pair (i, j);
					zeroes.push(toPush);
				}
				if(gameBoard[i][j] == '.'){ //adjacency hasn't been revealed. Reveal
					gameBoard[i][j] = adjacentBoard[i][j];
					tally++;
				}
			}
		}
	}
}

void Board::print(vector<vector<char> > vect) {
	for (vector<vector<int> >::size_type x = 0; x < vect.size(); x++) {
		for (vector<int>::size_type y = 0; y < vect[x].size(); y++) {
			cout << setw(4) << vect[x][y] << ' ';
		}
		cout << endl;
		cout << endl;
	}
}

void Board::generate() {
	realBoard.resize(size + 2, vector<char>(size + 2, 'F'));
	int count = 0;
	while (count < mines) {
		int x = rand() % (size - 1) + 1;
		int y = rand() % (size - 1) + 1;
		if (realBoard[x][y] == 'F') {
			realBoard[x][y] = 'T';
			count++;
		}
	}

	adjacentBoard.resize(size + 2, vector<char>(size + 2, ' '));
	for (int i = 1; i <= size; i++) {
		for (int j = 1; j <= size; j++) {
			adjacentBoard[i][j] = checkAdjacent(i, j);
		}
	}
}

void Board::display() {
	for (vector<vector<int> >::size_type x = 0; x < realBoard.size(); x++) {
		for (vector<int>::size_type y = 0; y < realBoard[x].size(); y++) {
			cout << setw(4) << realBoard[x][y] << ' ';
		}
		cout << endl;
		cout << endl;
	}
	for (vector<vector<int> >::size_type x = 0; x < adjacentBoard.size(); x++) {
		for (vector<int>::size_type y = 0; y < adjacentBoard[x].size(); y++) {
			cout << setw(4) << adjacentBoard[x][y] << ' ';
		}
		cout << endl;
		cout << endl;
	}
}

char Board::checkAdjacent(int i, int j) { //returns count of mines surrounding the coordinate
	int count = 0;
	if (realBoard[i][j - 1] == 'T')
		count++;
	if (realBoard[i + 1][j - 1] == 'T')
		count++;
	if (realBoard[i + 1][j] == 'T')
		count++;
	if (realBoard[i + 1][j + 1] == 'T')
		count++;
	if (realBoard[i][j + 1] == 'T')
		count++;
	if (realBoard[i - 1][j + 1] == 'T')
		count++;
	if (realBoard[i - 1][j] == 'T')
		count++;
	if (realBoard[i - 1][j - 1] == 'T')
		count++;
	return (char) (count + '0');
}
