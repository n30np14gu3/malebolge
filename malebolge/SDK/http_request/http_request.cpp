#pragma comment (lib, "Wininet.lib")

#include <Windows.h>
#include <WinInet.h>
#include <string>

#include "http_request.h"

#define POST "POST"
#define GET "GET"
#define HEADER "Content-Type: application/x-www-form-urlencoded\r\n"




namespace http_request
{
	string post(
		const string& server,
		const string& route,
		const string& user_agent,
		const string& params,
		bool ssl
	)
	{
		const HINTERNET hInternet = InternetOpen(user_agent.c_str(), INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);

		if (hInternet != nullptr)
		{
			const HINTERNET hConnect = InternetConnect(hInternet, server.c_str(), ssl ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
			if (hConnect != nullptr)
			{
				const HINTERNET hRequest = HttpOpenRequest(hConnect, POST, route.c_str(), nullptr, nullptr, nullptr, ssl ? (INTERNET_FLAG_SECURE | INTERNET_FLAG_KEEP_CONNECTION) : INTERNET_SERVICE_HTTP, 0);
				if (hRequest != nullptr)
				{
					if (HttpSendRequest(hRequest, HEADER, static_cast<DWORD>(strlen(HEADER)), LPVOID(TEXT(params.c_str())), static_cast<DWORD>(params.length())))
					{
						unsigned char c = 0;
						string response;
						for (;;)
						{
							DWORD readed = 0;
							BOOL isRead = InternetReadFile(hRequest, LPVOID(&c), 1, &readed);
							if (readed == 0 || !isRead)
								break;

							response += c;
						}
						response += '\0';
						InternetCloseHandle(hRequest);
						InternetCloseHandle(hConnect);
						InternetCloseHandle(hInternet);
						return  response;
					}
				}
			}
			if(hConnect != nullptr)
				InternetCloseHandle(hConnect);
			InternetCloseHandle(hInternet);
		}
		if(hInternet != nullptr)
			InternetCloseHandle(hInternet);

		return "";
		
	}
	string get(
		const string& server,
		const string& route,
		const string& user_agent,
		const string& params,
		bool ssl
	)
	{
		HINTERNET hInternet = InternetOpen(user_agent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);

		if (hInternet != nullptr)
		{
			HINTERNET hConnect = InternetConnect(hInternet, server.c_str(), ssl ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
			if (hConnect != nullptr)
			{
				HINTERNET hRequest = HttpOpenRequest(hConnect, GET, (route + "?" + params).c_str(), nullptr, nullptr, nullptr, ssl ? (INTERNET_FLAG_SECURE | INTERNET_FLAG_KEEP_CONNECTION) : INTERNET_SERVICE_HTTP, 0);
				if (hRequest != nullptr)
				{
					if (HttpSendRequest(hRequest, nullptr, 0, nullptr, 0))
					{
						unsigned char c = 0;
						string response;
						for (;;)
						{
							DWORD readed = 0;
							BOOL isRead = InternetReadFile(hRequest, LPVOID(&c), 1, &readed);
							if (readed == 0 || !isRead)
								break;

							response += c;
						}
						response += '\0';
						InternetCloseHandle(hRequest);
						InternetCloseHandle(hConnect);
						InternetCloseHandle(hInternet);
						return  response;
					}
				}
			}
			if(hConnect != nullptr)
				InternetCloseHandle(hConnect);
			InternetCloseHandle(hInternet);
		}
		if(hInternet != nullptr)
			InternetCloseHandle(hInternet);

		return "";
	}
}