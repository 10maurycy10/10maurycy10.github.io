board = [0 for _ in range(9)]

def checkwins(board):
	# A table of all possible combinations of squares that make up a 3 long line
	win_table = [[0,1,2],[3,4,5],[6,7,8],[0,3,6],[1,4,7],[2,5,6],[0,4,8],[2,4,6]]
	for win in win_table:
		# If all tiles in a win condidtition are the same, it is a win
		markers = [board[i] for i in win]
		if markers[0] == markers[1] and markers[0] == markers[2] and markers[0] != 0:
			return markers[0]

def makemove(board,move,player):
	newboard = board.copy()
	newboard[move] = player
	return newboard

def getvalidmoves(board):
	moves = []
	for i in range(9):
		if board[i] == 0:
			moves.append(i)
	return moves

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

player = 1
ai = -1
human = 1
while len(getvalidmoves(board)) > 0:
    printboard(board)
    if checkwins(board):
        break;
    if player == human:
        move = int(input("Enter move: "))
        board[move] = player
    if player == ai:
        (rank, move) = minmax(board,player)
        board[move] = player
    player = -player



