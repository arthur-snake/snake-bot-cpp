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

	template <class Callable>
	int connect(string url, Callable callable) {

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

		cout << "Connecting to " << url << endl;

		Snake *snake = new Snake();

	    ws = WebSocket::from_url(url);
	    assert(ws);
	    while (ws->getReadyState() != WebSocket::CLOSED) {
	    	ws->poll();
	    	ws->dispatch([snake, ws, callable](const string& msg) {
	    		snake->onMessage(msg, callable, [ws](string msg) {
	    			ws->send(msg);
	    		});
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