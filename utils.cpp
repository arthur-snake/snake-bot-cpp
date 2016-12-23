#include "snake.cpp"

using namespace std;

const int DN = 4;
const int DX[DN] = {-1, 0, 1, 0};
const int DY[DN] = {0, -1, 0, 1};
const string GO_MSG[DN] = {"LEFT", "UP", "RIGHT", "DOWN"};

struct BfsNode {
	bool visited = false;
	bool start = false;
	int dir = -1;
	int fx = -1, fy = -1; //ancestor location
};

/**
 *	Print map to console. (cy; cy) - current snake head location
 */ 
void printMapDebug(SnakeMap& map, SnakeIds& ids, int cx, int cy) {
	for (int i = 0; i < map.rows; i++) {
		for (int j = 0; j < map.columns; j++) {
			if (i == cy && j == cx) {
				cout << "X";
				continue;
			}
			ObjectInfo obj = ids[map[i][j].id];
			if (obj.type == "food") {
				cout << "6";
				continue;
			}
			if (obj.type == "free") {
				cout << ".";
				continue;
			}
			cout << "#";
		}
		cout << endl;
	}
	cout << endl;
}

/**
 *	Bfs to the nearest cell with food
 */
pair<vector<vector<BfsNode>>, vector<pair<int, int>>> bfs(SnakeMap& map, SnakeIds& ids, int cx, int cy) {
	int rows = map.rows;
	int columns = map.columns;
	vector<vector<BfsNode>> t(rows, vector<BfsNode>(columns));

	t[cy][cx].visited = true;
	t[cy][cx].start = true;

	vector<pair<int, int> > q;
	q.push_back(make_pair(cy, cx));

	int it = 0;
	while (it < q.size()) {
		int x, y;
		tie(y, x) = q[it++];

		for (int i = 0; i < DN; i++) {
			int nx = x + DX[i];
			int ny = y + DY[i];

			if (nx < 0) nx += columns;
			if (ny < 0) ny += rows;
			if (nx >= columns) nx -= columns;
			if (ny >= rows) ny -= rows;

			string toId = map[ny][nx].id;
			auto obj = ids[toId];

			if ((obj.type != "free" && obj.type != "food") || t[ny][nx].visited) continue;

			q.push_back(make_pair(ny, nx));
			t[ny][nx].visited = true;
			t[ny][nx].fx = x;
			t[ny][nx].fy = y;
			t[ny][nx].dir = i;
		}
	}
	return make_pair(t, q);
}