#include <stdio.h>
// 1 = x
// 0 = ' '
// -1 = o

// (0) (1) (2)
// (3) (4) (5)
// (6) (7) (8)

// Ansi escape codes added for nice rendering
char* numbers[10] = {
	"\e[0;34m0\e[0m",
	"\e[0;34m1\e[0m",
	"\e[0;34m2\e[0m",
	"\e[0;34m3\e[0m",
	"\e[0;34m4\e[0m",
	"\e[0;34m5\e[0m",
	"\e[0;34m6\e[0m",
	"\e[0;34m7\e[0m",
	"\e[0;34m8\e[0m",
	"\e[0;34m9\e[0m",
};

// Formats a 
void print_board(int board[9]) {
	char* x = "X";
	char* o = "O";
	char* c[9];
	for (int i = 0; i<9; i++) {
		switch (board[i]) {
			case 1:
				c[i] = x;
				break;
			case -1:
				c[i] = o;
				break;
			case 0:
				c[i] = numbers[i];
				break;
		}
	}
	printf("-- BOARD STATE --\n");
	printf("%s|%s|%s\n", c[0], c[1], c[2]);
	printf("-|-|-\n");
	printf("%s|%s|%s\n", c[3], c[4], c[5]);
	printf("-|-|-\n");
	printf("%s|%s|%s\n", c[6], c[7], c[8]);
}

// Checked
int check_wins(int board[9]) {
	int win_table[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,6},{0,4,8},{2,4,6}};
	for (int i = 0; i<8; i++) {
		int mark = board[win_table[i][0]];
		if (mark == board[win_table[i][1]] && mark == board[win_table[i][2]]) {
			return mark;
		}
	}
	return 0;
}

float rank_board(int board[9], int turn) {
	// If the board is a win, assume it is o 
	int win = check_wins(board);
	if (win != 0) {
		return win*turn;
	}
	// Assume ties are better than losses, but worse than wins.
	float best_ranking = 0;
	float movecount = 0;
	float totalscore = 0;
	// For all valid moves.
	for (int move = 0; move<9; move++) {
		if (board[move] == 0) {
			movecount++;
			// Make move and check board ranking, then undo it.
			board[move] = turn;
			float ranking = -rank_board(board, -turn);
			totalscore += ranking;
			board[move] = 0;
			// If the rank is better, replace best_ranking.
			if (ranking > best_ranking) {
				best_ranking = ranking;
			}
		}
	}
	// Add a small chance that the player plays suboptimaly.
	if (movecount > 0)
		return best_ranking*0.9 + (totalscore/movecount) * 0.1;
	else
		return best_ranking;
}

int get_best_move(int board[9], int turn) {
	// Assume ties are better than losses, but worse than wins.
	float best_ranking = 0;
	float best_move = -1;
	int got_move = 0;
	// For all valid moves
	for (int move = 0; move<9; move++) {
		if (board[move] == 0) {
			// Make move and check board ranking, then undo
			board[move] = turn;
			float ranking = -rank_board(board, -turn);
			board[move] = 0;
			// If ranking is better, or dont have a move, update best_ranking
			if ((ranking > best_ranking) || (!got_move)) {
				best_ranking = ranking;
				best_move = move;
				got_move = 1;
			}
		}
	}
	return best_move;
}

int main() {
	int board[9] = {0,0,0,0,0,0,0,0,0};
	int to_move = 1;
	int human = 0;
	while (human == 0) {
		printf("What do you want to play as? (y = 1, o = -1) ");
		scanf("%d", &human);
	}

	while (check_wins(board) == 0) {
		print_board(board);
		if (human == to_move) {
			while (1) {
				int move;
				printf("Your move, enter number: ");
				scanf("%d", &move);
				if (board[move] == 0) {
					board[move] = human;
					break;
				}
			}
		} else {
			int move = get_best_move(board, -human);
			printf("%d Making move\n", move);
			if (move == -1) {
				print_board(board);
				printf("Tie.\n");
				return 0;
			}
			board[move] = -human;

		}
		to_move *= -1;
	}
	int won = check_wins(board);
	print_board(board);
	switch (won) {
		case -1:
			printf("O wins.\n");
			break;
		case 1:
			printf("X wins.\n");
			break;
	}
	return 0;
}
