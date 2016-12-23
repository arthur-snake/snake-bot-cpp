#include "json.hpp"
#include <bits/stdc++.h>

using namespace std;

using json = nlohmann::json;

struct ObjectInfo { 
	string id; //object id
	string type; //object type. can be {"food", "player", "free", "block"}
	string color; //background color for cells
	string nick; //if type is player, this field contains player's nick
};

struct CellUpdate {
	int x;
	int y;
	string id;
	int food;
};

struct SnakeCell {
	string id; //cell id
	long time; //time(tick number) on which cell is updated (not supported yet)
	int food; //food avaliable in cell. If zero, cell has no food
};

class SnakeClient;
class Snake;

struct SnakeIds {
	/**
	 *	This map contains info about all game objects
	 */
	map<string, ObjectInfo> m;

	Snake* snake;

	void updateOne(json j);
	void updateMany(json j);

	/**
	 *	Access operator for objects
	 */
	ObjectInfo operator[] (string id);

	SnakeIds(Snake* snake): snake(snake) {}
};

struct SnakeMap {
	vector<vector<SnakeCell>> map;
	Snake* snake;

	int rows; //rows count (height of the field)
	int columns; //columns count (width of the field)

	/**
	 *	Client shouldn't use this function.
	 *	This function is processing cell update
	 */
	void updateOne(CellUpdate upd);

	/**
	 *	Access operator for map cells
	 */
	vector<SnakeCell>& operator[] (int y);

	SnakeMap(int rows, int columns, Snake* snake): 
		map(rows, vector<SnakeCell>(columns)), snake(snake), rows(rows), columns(columns) {}
};

struct Snake{
	function<void(string)> response;
	SnakeClient* client;

	SnakeIds ids;
	SnakeMap map;

	/**
	 *	Client shouldn't use this function. 
	 *	This function is processing server tick
	 */
	void onMessage(const string& message);

	/**
	 *	Join game with specified nick
	 */
	void join(string nick);

	/**
	 *	Change snake direction
	 */
	void go(string dir);

	/**
	 *	Leave game
	 */
	void leave();

	/**
	 *	Send chat message. You must join game before send chat messages
	 */
	void chat(string s);

	Snake(function<void(string)> response, SnakeClient* client): response(response), client(client), map(0, 0, this), ids(this) {}
};

class SnakeClient {
public:

	/**
	 *	This function is called on every server update(tick)
	 */
	virtual void onUpdate(Snake* snake, const json& j) = 0;

	/**
	 *	Init(first) message from the server
	 */
	virtual void onInit(Snake* snake, const SnakeIds& ids, const SnakeMap& map) = 0;

	/**
	 *	Information about new object(in most cases that object has just joined the game)
	 */
	virtual void onNewObject(Snake* snake, ObjectInfo objectInfo) = 0;

	/**
	 *	Cell on map is updated
	 */
	virtual void onCellUpdate(Snake* snake, int x, int y, CellUpdate upd, ObjectInfo info) = 0;
};
