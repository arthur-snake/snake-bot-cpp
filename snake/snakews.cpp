#include "../libs/easywsclient.hpp"
#include "../libs/easywsclient.cpp"
#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif

#include "snake.cpp"
#include <bits/stdc++.h>

using namespace std;

namespace SnakeWS{

	using easywsclient::WebSocket;

	/**
	 *	Handle connection between server and client while websocket is open
	 */
	int connect(string url, SnakeClient* client) {

		WebSocket::pointer ws = NULL;

		#ifdef _WIN32
			INT rc;
			WSADATA wsaData;

			rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (rc) {
				cerr << "WSAStartup Failed." << endl;
				return 1;
			}
		#endif

		ws = WebSocket::from_url(url);
		assert(ws);

		Snake *snake = new Snake([ws](string msg) {
			ws->send(msg);
			ws->poll();
		}, client);

		while (ws->getReadyState() != WebSocket::CLOSED) {
			ws->poll(1000);
			ws->dispatch([snake](const string& msg) {
				snake->onMessage(msg);
			});
		}
		delete ws;
		delete snake;

		#ifdef _WIN32
			WSACleanup();
		#endif

		return 0;
	}
};