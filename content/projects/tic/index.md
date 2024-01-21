---
title: "Analyzing Tic Tac Toe, and writing an AI."
summary: "Using the min max algorithm to make a Tic Tac Toe AI."
date: 2022-10-04T12:40:28-07:00
tags: ["games", "programming", "ai"]
draft: false
---

# The game

[Tic Tac Toe](https://en.wikipedia.org/wiki/Tic-tac-toe) is a fairly simple 2 player game played on a 3*3 board.

The players take turns placing a marker (typically X for player one and O for player 2) on a unoccupied space, and the first player that gets 3 in a line, including diagonals, wins.

Game in progress, X to move:

```
|O| | |
|O| | |
|X| |X|
```

# Min Max

One simple way to find the best move in a game is with the [min max](https://en.wikipedia.org/wiki/Minimax) algorithm.

Min max works by assigning each possible game states a score based how how favorable they are for the player.

Win conditions are easy, but for other states require the algorithm to recurse, assuming each player makes the most favorable move.

# Storing the board

I have decided to store the board as an array of 9 ints (row major).

The first player ('X') is 1 and the second ('O') is -1.

```py
board = [0 for _ in range(9)]
```

# Detecting wins

```py
def checkwins(board):
	# A table of all possible combinations of squares that make up a 3 long line
	win_table = [[0,1,2],[3,4,5],[6,7,8],[0,3,6],[1,4,7],[2,5,6],[0,4,8],[2,4,6]]
	for win in win_table:
		# If all tiles in a win condidtition are the same, it is a win
		markers = [board[i] for i in win]
		if markers[0] == markers[1] and markers[0] == markers[2] and markers[0] != 0:
			return markers
```

# Min Max

Now we can now layout the min max function:

```py
# Compute score for a board, from the to move players POV.
def minmax(board, player):
	# Trival case, one player wins
	win = checkwins(board)
	if win != None:
		return (win * player, None)
	# For all possible moves, find the best ranked one
	bestrank = None;
	bestmove = None
	for move in getvalidmoves(board):
		newboard = makemove(board,move,player)
		(rank, _) = minmax(newboard, -player)
		rank = -rank
		if bestrank == None or rank > bestrank:
			bestrank = rank
			bestmove = move
	# If there are no valid moves, assume tie, and rank as 0.
	return (bestrank or 0, bestmove)
```

The get valid moves functions simply returns all squares with a value of zero.

```py
def getvalidmoves(board):
	moves = []
	for i in range(9):
		if board[i] == 0:
			moves.append(i)
	return moves
```

Finally, the makemove function:

```py
def makemove(board,move,player):
	newboard = board.copy()
	newboard[move] = player
	return newboard
```

Putting it all together:

```py
def checkwins(board):
	# A table of all possible combinations of squares that make up a 3 long line
	win_table = [[0,1,2],[3,4,5],[6,7,8],[0,3,6],[1,4,7],[2,5,6],[0,4,8],[2,4,6]]
	for win in win_table:
		# If all tiles in a win condidtition are the same, it is a win
		markers = [board[i] for i in win]
		if markers[0] == markers[1] and markers[0] == markers[2] and markers[0] != 0:
			return markers

def getvalidmoves(board):
	moves = []
	for i in range(9):
		if board[i] == 0:
			moves.append(i)
	return moves

def makemove(board,move,player):
	newboard = board.copy()
	newboard[move] = player
	return newboard

# Compute score for a board, from the to move players POV.
def minmax(board, player):
	# Trival case, one player wins
	win = checkwins(board)
	if win != None:
		return (win * player, None)
	# For all possible moves, find the best ranked one
	bestrank = None;
	bestmove = None
	for move in getvalidmoves(board):
		newboard = makemove(board,move,player)
		(rank, _) = minmax(newboard, -player)
		rank = -rank
		if bestrank == None or rank > bestrank:
			bestrank = rank
			bestmove = move
	# If there are no valid moves, assume tie, and rank as 0.
	return (bestrank or 0, bestmove)
```

# Tic Tac Toe AI

A function to nicely format the board would be nice.

```py
def printboard(board):
	b = []
	for (i, v) in enumerate(board):
		if v == 1:
			b.append("X")
		elif v == -1:
			b.append("O")
		else:
			b.append(str(i))
		print()
		print(f"{b[0]}|{b[1]}|{b[2]}")
		print(f"{b[3]}|{b[4]}|{b[5]}")
		print(f"{b[6]}|{b[7]}|{b[8]}")
```

And finally, the game loop:

```py
player = 1
ai = -1
human = 1
while len(getvalidmoves(board)) > 0:
	printboard(board)
	if checkwins(board):
		break
	if player == human:
		move = int(input("Enter move: "))
		board[move] = player
	if player == ai:
		(rank, move) = minmax(board,player)
		board[move] = player
	player = -player
```

# Accounting for imperfect players

If you play around with this version, you may notice it doesn't try to proactively win, this is because it assumes that you are playing perfectly, and wont fall for any tricks.

A easy modification to make would be to have it assumes you play perfectly 90% of the time, and randomly 10% of the time.

```py
boardrank = minmaxRank*0.9 + averageRank*0.1
```

The resulting min max function looks like this:

```py
# Compute score for a board, from the to move players POV.
def minmax(board, player):
    # Trival case, one player wins
    win = checkwins(board)
    if win != None:
        return (win * player, None)
    # For all possible moves, find the best ranked one
    bestrank = None;
    bestmove = None;
    ranks = []
    for move in getvalidmoves(board):
        newboard = makemove(board,move,player)
        (rank, _) = minmax(newboard, -player)
        rank = -rank
        ranks.append(rank)
        if bestrank == None or rank > bestrank:
            bestrank = rank
            bestmove = move
    # If there are no valid moves, assume tie, and rank as 0.
    if len(ranks) == 0:
        return (0, None)
    else:
        return ((bestrank * 0.9) + (sum(ranks)/len(ranks) * 0.1), bestmove)
```

# Finished code

You can find all the python code (ready to run) [here](tic.py)

[I have also rewritten it in C, with UI improvements](tic.c)

# Going further

I have made a list of suggestions if you chose to play around with this

- Have the computer ask what player you want to play.

- Allow input of an arbitrary board

- Make sure the human has entered a legal move

- Improve contrast, make it easier to spot empty squares
