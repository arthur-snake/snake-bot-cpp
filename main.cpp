#include "snakews.cpp"
#include <bits/stdc++.h>

using namespace std;

void handle_message(const string& message) {
    printf(">>> %s\n", message.c_str());
}

int main() {
    while (true) {
        SnakeWS::connect("ws://wrt.qjex.xyz:8080/snake/ws/small", handle_message);
    }
    return 0;
}
