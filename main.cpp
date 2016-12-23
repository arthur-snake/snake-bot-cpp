#include "snakews.cpp"
#include <bits/stdc++.h>
#include "json.hpp"
#include "utils.cpp"

using json = nlohmann::json;

using namespace std;

class MyStrategy: public SnakeClient {
public:

	string nick;
	string myId;

	int cx, cy;

	void onUpdate(Snake* snake, const json& j) override {
	    //cerr << j.dump(4) << endl;

		if (cx == -1 || cy == -1) return;

		SnakeMap& map = snake->map;
		SnakeIds& ids = snake->ids;

        auto res = bfs(map, ids, cx, cy);
        auto field = res.first;
        auto q = res.second;
        if (q.size() < 2) return;

        int x, y;
        tie(y, x) = q.back();
        for (int i = 1; i < q.size(); i++) {
            int tx, ty;
            tie(ty, tx) = q[i];
            if (ids[map[ty][tx].id].type == "food") {
                x = tx;
                y = ty;
                break;
            }
        }

        while (true) {
            int fx = field[y][x].fx;
            int fy = field[y][x].fy;

            assert(fx != -1 && fy != -1);

            if (field[fy][fx].start) {
                snake->go(GO_MSG[field[y][x].dir]);
                return;
            }
            y = fy;
            x = fx;
        }
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

		if (obj.type == "player" && obj.nick == nick) { //if new object has the same nick as bot
			myId = obj.id; //then bot id is that object id
			cerr << "Player id => " << myId << endl;
		}
	}

	void onCellUpdate(Snake* snake, int x, int y, CellUpdate upd, ObjectInfo info) override {
		if (upd.id == myId) {
			cx = x;
			cy = y;
			//cerr << "head position: " << y << " " << x << endl;
		}
	}

	MyStrategy(): cx(-1), cy(-1) {}
};

int main() {
    while (true) {
    	MyStrategy strategy;
        SnakeWS::connect("ws://wrt.qjex.xyz:8080/snake/ws/faster", &strategy);
    }
    return 0;
}
