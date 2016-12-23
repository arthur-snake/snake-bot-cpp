#include "json.hpp"
#include <bits/stdc++.h>
#pragma once
#include "snake.h"

using namespace std;

using json = nlohmann::json;

void SnakeMap::updateOne(CellUpdate upd) {
	//cerr << "UPDATE(" << upd.y << ", " << upd.x << ", " << upd.id << ", " << upd.food << ")" << endl;
	map[upd.y][upd.x] = {upd.id, 0, upd.food};
	snake->client->onCellUpdate(snake, upd.x, upd.y, upd, snake->ids[upd.id]);
}

vector<SnakeCell>& SnakeMap::operator[] (int y) {
	return this->map[y];
}

ObjectInfo createObjectInfo(json j) {
	ObjectInfo obj;
	obj.id = j["id"];
	obj.type = j["type"];
	obj.color = j["color"];
	if (j["nick"].is_string()) obj.nick = j["nick"];
	return obj;
}

void SnakeIds::updateOne(json j) {
	//cerr << "new id: " << j.dump(1) << endl;
	ObjectInfo obj = createObjectInfo(j);
	m[obj.id] = obj;

	snake->client->onNewObject(snake, obj);
}

void SnakeIds::updateMany(json j) {
	if (!j.is_array()) return;
	for (auto i : j) updateOne(i);
}

CellUpdate parseMapEvent(string s) {
	CellUpdate upd;

	int pos1 = s.find('.');
	int pos2 = s.find('#');
	int pos3 = s.find('*');
	if (pos3 != -1) {
		upd.food = atoi(s.substr(pos3 + 1).data());
	} else {
		upd.food = 0;
		pos3 = s.size();
	}

	upd.x = atoi(s.substr(0, pos1).data());
	upd.y = atoi(s.substr(pos1 + 1, pos2 - pos1 - 1).data());
	upd.id = s.substr(pos2 + 1, pos3 - pos2 - 1);

	return upd;
}

vector<CellUpdate> parseMapEvents(json j) {
	vector<CellUpdate> res; 
	if (!j.is_string()) return res;
	string s = j;
	string buf;
	for (int i = 0; i <= s.size(); i++) {
		if (i == s.size() || s[i] == '|') {
			res.push_back(parseMapEvent(buf));
			buf = "";
		} else buf += s[i];
	}
	return res;
}

void Snake::onMessage(const string& message) {
	json j = json::parse(message);

	if (j["act"].is_string()) {
		string act = j["act"];
		if (act == "init") {
			int rows = j["rows"];
			int columns = j["columns"];

			cerr << "Server map size is " << rows << " " << columns << endl;

			ids = SnakeIds(this);
			map = SnakeMap(rows, columns, this);

			client->onInit(this, ids, map);
		}
	}

	ids.updateMany(j["u"]);
	vector<CellUpdate> upds = parseMapEvents(j["a"]);
	for (auto upd : upds) map.updateOne(upd);

	client->onUpdate(this, j);
}

void Snake::join(string nick) {
	json j = {
		{"act", "join"},
		{"nick", nick}
	};
	response(j.dump());
}

void Snake::go(string dir) {
	json j = {
		{"act", "turn"},
		{"dir", dir}
	};
	response(j.dump());
}

void Snake::leave() {
	response("{\"act\":\"leave\"}");
}

void Snake::chat(string msg) {
	json j = {
		{"act", "chat"},
		{"msg", msg}
	};
	response(j.dump());
}