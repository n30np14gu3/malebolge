#pragma once

#define LOCAL_IP_S "127.0.0.1"

#ifndef NDEBUG
#define LOCAL_IP LOCAL_IP_S
#else
#define LOCAL_IP LOCAL_IP_S
#endif


class local_proto
{
public:
	int NoError = 0;

	local_proto(u_short port);
	~local_proto();
	bool verification();
private:

	bool sendPacket(byte* packet, int packetSize, bool crypt = false);
	byte* recivePacket(int& packetSize, bool crypt = false);

	WSADATA m_wsaData{};
	SOCKET m_sClient;
	sockaddr_in m_sockAddr{};
	int m_iErrorCode;
	byte* pKey{};
};