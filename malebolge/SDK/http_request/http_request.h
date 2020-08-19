#pragma once
namespace http_request
{
	using namespace std;

	string post(
		const string& server,
		const string& route,
		const string& user_agent,
		const string& params,
		bool ssl
	);

	string get(
		const string& server,
		const string& route,
		const string& user_agent,
		const string& params,
		bool ssl
	);
}