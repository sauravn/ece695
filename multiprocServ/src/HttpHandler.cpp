/*
 * HttpHandler.cpp
 *
 *  Created on: Apr 12, 2015
 *      Author: yzhh
 */

#include "HttpHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <sys/socket.h>
#include <iostream>
#include <cstring>

using namespace std;

HttpHandler::HttpHandler(int sock) : data_read(0), cli_sock(sock)
{
	//this->buf = {'\0'};
	memset(buf, '\0', BUF_SIZE);
}


void HttpHandler::run()
{
	while(1) {
		data_read = recv(cli_sock, buf, BUF_SIZE, 0);
		if (data_read == -1) {
			cout << "reading error -1" << endl;
			break;
		} else if ( data_read == 0) {
			cout << "read finish, client closed connection" << endl;
			return;
		} else {
			cout << "got " << data_read << "data" << endl;
			cout << buf << endl;
			HttpRequest req(buf, data_read);
			cout << req.getMethod() << endl;
			if (req.getMethod() == GET) {
				HttpResponse resp;
				char *data = resp.to_rawdata();
				send(cli_sock, data, strlen(data), 0);
			}
			memset(buf, '\0', BUF_SIZE);
		}
	}

}

