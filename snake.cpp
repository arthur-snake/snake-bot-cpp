#include <bits/stdc++.h>

using namespace std;

struct Snake{

	template <class Callable, class Response>
	void onMessage(const string& message, Callable callable, Response response) {
		cerr << message << endl;
	}
};