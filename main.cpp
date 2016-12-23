#include "snakews.cpp"
#include <bits/stdc++.h>

#include "json.hpp"
using json = nlohmann::json;

using namespace std;

const int DN = 4;
const int dx[DN] = {-1, 0, 1, 0};
const int dy[DN] = {0, -1, 0, 1};
const string msg[DN] = {"LEFT", "UP", "RIGHT", "DOWN"};

struct BfsNode {
	bool visited = false;
	bool start = false;

	int dir = -1;

	int fx = -1, fy = -1; //ancestor location
};

class MyStrategy: public SnakeClient {
public:

	string nick;
	string myId;

	int cx, cy;

	vector<vector<BfsNode>> t;

	void printMapDebug(SnakeMap& map, SnakeIds& ids) {
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

	void bfs(Snake* snake, SnakeMap& map, SnakeIds& ids) {
		//printMapDebug(map, ids);

		int rows = map.rows;
		int columns = map.columns;
		t.assign(rows, vector<BfsNode>(columns, BfsNode()));

		t[cy][cx].visited = true;
		t[cy][cx].start = true;

		queue<pair<int, int> > q;
		q.push(make_pair(cy, cx));

		int tx = -1;
		int ty = -1;

		int lx, ly;

		while (!q.empty()) {
			int x, y;
			tie(y, x) = q.front();
			q.pop();

			lx = x;
			ly = y;

			if (map[y][x].food != 0) {
				tx = x;
				ty = y;
				break;
			}

			for (int i = 0; i < DN; i++) {
				int nx = x + dx[i];
				int ny = y + dy[i];

				if (nx < 0) nx += columns;
				if (ny < 0) ny += rows;
				if (nx >= columns) nx -= columns;
				if (ny >= rows) ny -= rows;

				string toId = map[ny][nx].id;
				auto obj = ids[toId];

				if ((obj.type != "free" && obj.type != "food") || t[ny][nx].visited) continue;

				q.push(make_pair(ny, nx));
				t[ny][nx].visited = true;
				t[ny][nx].fx = x;
				t[ny][nx].fy = y;
				t[ny][nx].dir = i;
			}
		}

		if (tx == -1 || ty == -1) {
			if (t[ly][lx].start) return;
			tx = lx;
			ty = ly;
		}

		while (true) {
			int fx = t[ty][tx].fx;
			int fy = t[ty][tx].fy;

			if (fx == -1 || fy == -1) break;

			if (t[fy][fx].start) {
				//cerr << "GO " << msg[t[ty][tx].dir] << endl;
				snake->go(msg[t[ty][tx].dir]);
				return;
			}
			ty = fy;
			tx = fx;
		}
	}

	void onUpdate(Snake* snake, const json& j) override {
	    //cerr << j.dump(4) << endl;

		if (cx == -1 || cy == -1) return;

		SnakeMap& map = snake->map;
		SnakeIds& ids = snake->ids;

		bfs(snake, map, ids);
	}

	void onInit(Snake* snake, const SnakeIds& snakeIds, const SnakeMap& snakeMap) override {
		nick = "C++ Bot #";
		for (int i = 0; i < 6; i++) {
			nick += '0' + (rand() % 10);
		}
		snake->join(nick);
		snake->chat("Hello from C++!");
	}

	void onNewObject(Snake* snake, ObjectInfo obj) override {

		// cerr << "new object(id=" << obj.id << ", type=" << obj.type;
		// if (obj.type == "player") cerr << ", nick=" << obj.nick;
		// cerr << ")" << endl;

		if (obj.type == "player" && obj.nick == nick) {
			myId = obj.id;
			cerr << "Player id => " << myId << endl;
		}
	}

	void onCellUpdate(Snake* snake, int x, int y, CellUpdate upd, ObjectInfo info) override {
		//assert(info.id == upd.id);
		if (upd.id == myId) {
			cx = x;
			cy = y;
			//assert(snake->map[y][x].id == myId);
			//cerr << "head position: " << y << " " << x << endl;
		}
	}

	MyStrategy(): cx(-1), cy(-1) {}
};

int main() {
    while (true) {
    	MyStrategy strat;
        SnakeWS::connect("ws://wrt.qjex.xyz:8080/snake/ws/faster", &strat);
    }
    return 0;
}
