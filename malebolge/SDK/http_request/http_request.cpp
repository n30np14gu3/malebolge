#pragma comment (lib, "Wininet.lib")

#include <Windows.h>
#include <WinInet.h>
#include <string>

#include "../VMProtectSDK.h"
#include "http_request.h"

#define POST_S		"POST"
#define GET_S		"GET"
#define HEADER_S	"Content-Type: application/x-www-form-urlencoded\r\n"

#ifndef NDEBUG
#define POST	POST_S
#define GET		GET_S
#define HEADER	HEADER_S
#else
#define POST		VMProtectDecryptStringA(POST_S)
#define GET			VMProtectDecryptStringA(GET_S)
#define HEADER		VMProtectDecryptStringA(HEADER_S)
#endif



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
		const HINTERNET hInternet = InternetOpen(user_agent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);

		if (hInternet != nullptr)
		{
			const HINTERNET hConnect = InternetConnect(hInternet, server.c_str(), ssl ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
			if (hConnect != nullptr)
			{
				const HINTERNET hRequest = HttpOpenRequest(hConnect, POST, route.c_str(), nullptr, nullptr, nullptr, ssl ? (INTERNET_FLAG_SECURE | INTERNET_FLAG_KEEP_CONNECTION) : INTERNET_SERVICE_HTTP, 0);
				if (hRequest != nullptr)
				{
					if (HttpSendRequest(hRequest, HEADER, strlen(HEADER), LPVOID(TEXT(params.c_str())), params.length()))
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