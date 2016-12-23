#include "json.hpp"
#include <bits/stdc++.h>

using namespace std;

using json = nlohmann::json;

#pragma once

class SnakeClient;
class Snake;

struct ObjectInfo { 
	string id;
	string type;
	string color;
	string nick;
};

struct CellUpdate {
	int x;
	int y;
	string id;
	int food;
};

struct SnakeIds {
	map<string, ObjectInfo> m;

	Snake* snake;

	void updateOne(json j);
	void updateMany(json j);

	ObjectInfo operator[] (string id) {
		return m[id]; 
	}

	SnakeIds(Snake* snake): snake(snake) {}
};

struct SnakeCell {
	string id;
	long time;
	int food;
};

struct SnakeMap {
	vector<vector<SnakeCell>> map;
	Snake* snake;

	int rows;
	int columns;

	void updateOne(CellUpdate upd);

	vector<SnakeCell>& operator[] (int y);

	SnakeMap(int rows, int columns, Snake* snake): 
		map(rows, vector<SnakeCell>(columns)), snake(snake), rows(rows), columns(columns) {}
};

struct Snake{
	function<void(string)> response;
	SnakeClient* client;

	SnakeIds ids;
	SnakeMap map;

	void onMessage(const string& message);
	void join(string nick);
	void go(string dir);
	void leave();
	void chat(string s);

	Snake(function<void(string)> response, SnakeClient* client): response(response), client(client), map(0, 0, this), ids(this) {}
};

class SnakeClient {
public:
	virtual void onUpdate(Snake* snake, const json& j) = 0;
	virtual void onInit(Snake* snake, const SnakeIds& ids, const SnakeMap& map) = 0;
	virtual void onNewObject(Snake* snake, ObjectInfo objectInfo) = 0;
	virtual void onCellUpdate(Snake* snake, int x, int y, CellUpdate upd, ObjectInfo info) = 0;
};
