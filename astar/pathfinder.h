struct Path {
	vector<int> coords;
	vector<int> checkedCoords;
};

struct Tile {
	int x;
	int y;
	int id;
	int pid;
	float fcost;
	float gcost;
	float hcost;
};

#define X 0
#define Y 1

int adjacentTileOffsets[] = {
	0, -1, 10,
	-1, 0, 10,
	1, 0, 10,
	0, 1, 10,
	-1, -1, 14,
	1, -1, 14,
	-1, 1, 14,
	1, 1, 14
};

// Not written for optimal performance, but just for the concept.

Tile* newTile(int x, int y, int id, int pid, int gcost, int hcost) {

	Tile *tile = new Tile;
	tile->x = x;
	tile->y = y;
	tile->id = id;
	tile->pid = pid;
	tile->gcost = gcost;
	tile->hcost = hcost;
	tile->fcost = gcost + hcost;

	return tile;
};

void AppendTile(Tile* a[], Tile* t, int *size) {
	a[*size] = t;
	*size = *size+1;
}

void RemoveTile(Tile* a[], Tile* t, int *size) {

	int offset = sizeof(a);

	for (unsigned int i = 0; i < *size; ++i) {
		if (t->id == a[i]->id) offset = i;
	}

	for (unsigned int i = offset; i < *size; ++i) {
		if (i == *size) break;
		a[i] = a[i+1];
	}

	*size = *size-1;
}

int TileExists(Tile* a[], int *size, int x, int y) { 
	for (unsigned int i = 0; i < *size; ++i) {
		if (a[i]->x == x && a[i]->y == y) return i;
	}
	return -1;
}

Tile* GetTile(Tile* a[], int *size, int id) {

	if (id == 1) return NULL;

	for (unsigned int i = 0; i < *size; ++i) {
		if (a[i]->id == id) return a[i];
	}
	return NULL;
}

int HCost(int x, int y, int endX, int endY) {
	int xDif = abs(endX - x);
	int yDif = abs(endY - y);

	return (xDif+yDif) * 10;
}

template <int x, int y>
Path FindPath(int (&map)[x][y], int startX, int startY, int endX, int endY) {

	Tile* OpenList[BoardSize];
	Tile* ClosedList[BoardSize];
	int OpenListSize = 0;
	int ClosedListSize = 0;
	int IdIndex = 0;

	Path path;

	Tile* startTile = newTile(
		startX, 
		startY, 
		++IdIndex, 
		IdIndex, 
		0, HCost(startX, startY, endX, endY)
		);

	AppendTile(OpenList, startTile, &OpenListSize);

	bool done = false;
	while (!done) {

		Tile* ptile = NULL;

		if (OpenListSize == 0) break;
		for (unsigned int i = 0; i < OpenListSize; ++i) {
			if (ptile == NULL) ptile = OpenList[i];
			else if (OpenList[i]->fcost < ptile->fcost) ptile = OpenList[i];
		}

		RemoveTile(OpenList, ptile, &OpenListSize);
		AppendTile(ClosedList, ptile, &ClosedListSize);

		if (ptile->x == endX && ptile->y == endY) {
			Tile* t = ptile;
			while (t) {
				t = GetTile(ClosedList, &ClosedListSize, t->pid);
				if (t) {
					path.coords.push_back(t->x);
					path.coords.push_back(t->y);
				}
			}

			for (unsigned int i = 0; i < ClosedListSize; ++i) {
				path.checkedCoords.push_back(ClosedList[i]->x);
				path.checkedCoords.push_back(ClosedList[i]->y);
			}
			break;
		}

		for (unsigned int i = 0; i < (sizeof(adjacentTileOffsets)/sizeof(adjacentTileOffsets[0]))/3; ++i) {

			int x = ptile->x + adjacentTileOffsets[(i*3)];
			int y = ptile->y + adjacentTileOffsets[(i*3)+1];
			int cost = adjacentTileOffsets[(i*3)+2];
			int gcost = ptile->gcost + cost;

			if (x < 0 || y < 0 || x > BoardXSize || y > BoardYSize || map[x][y] == OCCUPIED) continue;
			if (TileExists(ClosedList, &ClosedListSize, x, y) > -1) continue;

			int tileIndex = TileExists(OpenList, &OpenListSize, x, y);
			if(tileIndex > -1) {

				// Is the path to this tile through the current tile a better path?
				if (gcost < OpenList[tileIndex]->gcost) {
					OpenList[tileIndex]->pid = ptile->id;
					OpenList[tileIndex]->gcost = gcost;
					OpenList[tileIndex]->fcost = gcost + OpenList[tileIndex]->hcost;
				}

			} else {
				Tile* tile = newTile(
					x, y,
					++IdIndex,
					ptile->id,
					gcost, HCost(x, y, endX, endY)
					);
				AppendTile(OpenList, tile, &OpenListSize);
			}
		}
	}

	return path;
}