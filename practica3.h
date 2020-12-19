


bool Map::movePlayer(int dx, int dy) {
	if (lvlMap[playerY + dy][playerX + dx] != '#')
		if (moveBox(playerX + dx, playerY + dy, dx, dy)) {
			playerFromBlank(playerX, playerY, dx, dy);
			playerFromGoal(playerX, playerY, dx, dy);
			playerX += dx;
			playerY += dy;
			return true;
		}
	return false;
}

bool Map::moveBox(int boxX, int boxY, int dx, int dy) {
	if (lvlMap[boxY][boxX] == '$' || lvlMap[boxY][boxX] == '*') {
		if (lvlMap[boxY + dy][boxX + dx] == '$' || lvlMap[boxY + dy][boxX + dx] == '#' || lvlMap[boxY + dy][boxX + dx] == '*') {
			return false;
		}
		boxFromBlank(boxX, boxY, dx, dy);
		boxFromGoal(boxX, boxY, dx, dy);
	}
	return true;
}

void Map::boxFromBlank(int x, int y, int dx, int dy) {
	if (lvlMap[y][x] == '$') {
		lvlMap[y][x] = ' ';
		if (lvlMap[y + dy][x + dx] == '.') {
			lvlMap[y + dy][x + dx] = '*';
			goalPlaces--;
		}
		else
			lvlMap[y + dy][x + dx] = '$';
	}
}

void Map::boxFromGoal(int x, int y, int dx, int dy) {
	if (lvlMap[y][x] == '*') {
		lvlMap[y][x] = '.';
		if (lvlMap[y + dy][x + dx] == '.') {
			lvlMap[y + dy][x + dx] = '*';
		}
		else {
			lvlMap[y + dy][x + dx] = '$';
			goalPlaces++;
		}
	}
}

void Map::playerFromBlank(int x, int y, int dx, int dy) {
	if (lvlMap[y][x] == '@') {
		lvlMap[y][x] = ' ';
		if (lvlMap[y + dy][x + dx] == '.')
			lvlMap[y + dy][x + dx] = '+';
		else
			lvlMap[y + dy][x + dx] = '@';
	}
}

void Map::playerFromGoal(int x, int y, int dx, int dy) {
	if (lvlMap[y][x] == '+') {
		lvlMap[y][x] = '.';
		if (lvlMap[y + dy][x + dx] == '.')
			lvlMap[y + dy][x + dx] = '+';
		else
			lvlMap[y + dy][x + dx] = '@';
	}
}