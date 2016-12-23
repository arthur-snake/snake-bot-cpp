#include "easywsclient.hpp"
#include "easywsclient.cpp"
#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif

#include "snake.cpp"

#include <bits/stdc++.h>

using namespace std;

namespace SnakeWS{

	using easywsclient::WebSocket;

	int connect(string url, SnakeClient* client) {

		WebSocket::pointer ws = NULL;

		#ifdef _WIN32
		    INT rc;
		    WSADATA wsaData;

		    rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
		    if (rc) {
		        printf("WSAStartup Failed.\n");
		        return 1;
		    }
		#endif

	    ws = WebSocket::from_url(url);
	    assert(ws);

		Snake *snake = new Snake([ws](string msg) {
			ws->send(msg);
		}, client);

	    while (ws->getReadyState() != WebSocket::CLOSED) {
	    	ws->poll();
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