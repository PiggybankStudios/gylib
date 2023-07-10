/*
File:   gy_socket.h
Author: Taylor Robbins
Date:   07\09\2023
Description:
	** Holds functions that help us open, read, write, etc. to TCP\UDP sockets
*/

#ifndef _GY_SOCKET_H
#define _GY_SOCKET_H

#if SOCKETS_SUPPORTED
//NOTE: You must link Ws2_32.lib for this file to link successfully when SOCKETS_SUPPORTED

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
enum SocketType_t
{
	SocketType_None = 0,
	SocketType_Udp,
	SocketType_Tcp,
	SocketType_NumTypes,
};
const char* GetSocketTypeStr(SocketType_t enumValue)
{
	switch (enumValue)
	{
		case SocketType_None: return "None";
		case SocketType_Udp:  return "Udp";
		case SocketType_Tcp:  return "Tcp";
		default: return "Unknown";
	}
}

struct OpenSocket_t
{
	SocketType_t type;
	IpAddressAndPort_t destAddress;
	
	bool isOpen;
	
	#if WINDOWS_COMPILATION
	SOCKET handle_win32;
	#else
	#error Unsupported platform for gy_socket.h OpenSocket_t
	#endif
};

// +--------------------------------------------------------------+
// |                       Helper Functions                       |
// +--------------------------------------------------------------+
#if WINDOWS_COMPILATION
const char* Win32_GetWsaErrorStr(int wsaErrorCode)
{
	switch (wsaErrorCode)
	{
		case WSASYSNOTREADY:         return "WSASYSNOTREADY";
		case WSAVERNOTSUPPORTED:     return "WSAVERNOTSUPPORTED";
		case WSANOTINITIALISED:      return "WSANOTINITIALISED";
		case WSAEPROCLIM:            return "WSAEPROCLIM";
		case WSAEFAULT:              return "WSAEFAULT";
		case WSAENETDOWN:            return "WSAENETDOWN";
		case WSAEAFNOSUPPORT:        return "WSAEAFNOSUPPORT";
		case WSAEINPROGRESS:         return "WSAEINPROGRESS";
		case WSAEMFILE:              return "WSAEMFILE";
		case WSAEINVAL:              return "WSAEINVAL";
		case WSAEINVALIDPROVIDER:    return "WSAEINVALIDPROVIDER";
		case WSAEINVALIDPROCTABLE:   return "WSAEINVALIDPROCTABLE";
		case WSAENOBUFS:             return "WSAENOBUFS";
		case WSAEPROTONOSUPPORT:     return "WSAEPROTONOSUPPORT";
		case WSAEPROTOTYPE:          return "WSAEPROTOTYPE";
		case WSAEPROVIDERFAILEDINIT: return "WSAEPROVIDERFAILEDINIT";
		case WSAESOCKTNOSUPPORT:     return "WSAESOCKTNOSUPPORT";
		case WSAEACCES:              return "WSAEACCES";
		case WSAEINTR:               return "WSAEINTR";
		case WSAENETRESET:           return "WSAENETRESET";
		case WSAENOTCONN:            return "WSAENOTCONN";
		case WSAENOTSOCK:            return "WSAENOTSOCK";
		case WSAEOPNOTSUPP:          return "WSAEOPNOTSUPP";
		case WSAESHUTDOWN:           return "WSAESHUTDOWN";
		case WSAEWOULDBLOCK:         return "WSAEWOULDBLOCK";
		case WSAEMSGSIZE:            return "WSAEMSGSIZE";
		case WSAEHOSTUNREACH:        return "WSAEHOSTUNREACH";
		case WSAECONNABORTED:        return "WSAECONNABORTED";
		case WSAECONNRESET:          return "WSAECONNRESET";
		case WSAETIMEDOUT:           return "WSAETIMEDOUT";
		case WSAEADDRINUSE:          return "WSAEADDRINUSE";
		case WSAEALREADY:            return "WSAEALREADY";
		case WSAEADDRNOTAVAIL:       return "WSAEADDRNOTAVAIL";
		case WSAECONNREFUSED:        return "WSAECONNREFUSED";
		case WSAEISCONN:             return "WSAEISCONN";
		case WSAENETUNREACH:         return "WSAENETUNREACH";
		default: return "Unknown";
	}
}
#endif

#if WINDOWS_COMPILATION
sockaddr_in Win32_GetSockAddrFromIpAddressAndPort(IpAddressAndPort_t addressAndPort)
{
	sockaddr_in result = {};
	result.sin_family = (addressAndPort.address.isIpv6 ? AF_INET6 : AF_INET);
	result.sin_port = addressAndPort.port;
	if (addressAndPort.address.isIpv6)
	{
		sockaddr_in6* result6 = (sockaddr_in6*)&result;
		result6->sin6_flowinfo = 0;
		result6->sin6_addr.u.Word[0] = addressAndPort.address.ipv6.part0;
		result6->sin6_addr.u.Word[1] = addressAndPort.address.ipv6.part1;
		result6->sin6_addr.u.Word[2] = addressAndPort.address.ipv6.part2;
		result6->sin6_addr.u.Word[3] = addressAndPort.address.ipv6.part3;
		result6->sin6_addr.u.Word[4] = addressAndPort.address.ipv6.part4;
		result6->sin6_addr.u.Word[5] = addressAndPort.address.ipv6.part5;
		result6->sin6_addr.u.Word[6] = addressAndPort.address.ipv6.part6;
		result6->sin6_addr.u.Word[7] = addressAndPort.address.ipv6.part7;
	}
	else
	{
		result.sin_addr.S_un.S_un_b.s_b1 = addressAndPort.address.ipv4.part0;
		result.sin_addr.S_un.S_un_b.s_b2 = addressAndPort.address.ipv4.part1;
		result.sin_addr.S_un.S_un_b.s_b3 = addressAndPort.address.ipv4.part2;
		result.sin_addr.S_un.S_un_b.s_b4 = addressAndPort.address.ipv4.part3;
	}
	return result;
}
#endif

// +--------------------------------------------------------------+
// |                          Initialize                          |
// +--------------------------------------------------------------+
bool InitializeSockets()
{
	#if WINDOWS_COMPILATION
	{
		WSADATA startupData = {};
		int startupResult = WSAStartup(
		    MAKEWORD(2, 2), //wVersionRequired
			&startupData
		);
		if (startupResult != 0)
		{
			//TODO: WSASYSNOTREADY, WSAVERNOTSUPPORTED, WSAEINPROGRESS, WSAEPROCLIM, WSAEFAULT
			GyLibPrintLine_E("WSAStartup returned error: %s (%d)", Win32_GetWsaErrorStr(startupResult), startupResult);
			return false;
		}
	}
	#else
	#error Unsupported platform for gy_socket.h InitializeSockets
	#endif
	
	return true;
}

// +--------------------------------------------------------------+
// |                         Close Socket                         |
// +--------------------------------------------------------------+
void CloseOpenSocket(OpenSocket_t* socket)
{
	NotNull(socket);
	if (!socket->isOpen) { return; }
	
	#if WINDOWS_COMPILATION
	{
		Assert(socket->handle_win32 != INVALID_SOCKET);
		int closeResult = closesocket(socket->handle_win32);
		Assert(closeResult == 0); //TODO: Should we handle some kinds of errors? WSANOTINITIALISED, WSAENETDOWN, WSAENOTSOCK, WSAEINPROGRESS, WSAEINTR, WSAEWOULDBLOCK
		socket->handle_win32 = INVALID_SOCKET;
	}
	#else
	#error Unsupported platform for gy_socket.h CloseOpenSocket
	#endif
	
	socket->isOpen = false;
}

// +--------------------------------------------------------------+
// |                         Open Socket                          |
// +--------------------------------------------------------------+
bool TryOpenNewSocket(SocketType_t type, IpAddressAndPort_t destAddress, OpenSocket_t* socketOut)
{
	NotNull(socketOut);
	ClearPointer(socketOut);
	socketOut->type = type;
	socketOut->destAddress = destAddress;
	socketOut->isOpen = false;
	
	#if WINDOWS_COMPILATION
	{
		int win32_addressFamily = (destAddress.address.isIpv6 ? AF_INET6 : AF_INET);
		Assert(type == SocketType_Tcp || type == SocketType_Udp);
		int win32_type = ((type == SocketType_Tcp) ? SOCK_STREAM : SOCK_DGRAM);      //Other Values: SOCK_RAW, SOCK_RDM, SOCK_SEQPACKET
		int win32_protocol = ((type == SocketType_Tcp) ? IPPROTO_TCP : IPPROTO_UDP); //Other Values: IPPROTO_ICMP, IPPROTO_IGMP, BTHPROTO_RFCOMM, IPPROTO_ICMPV6, IPPROTO_RM
		
		socketOut->handle_win32 = socket(win32_addressFamily, win32_type, win32_protocol);
		if (socketOut->handle_win32 == INVALID_SOCKET)
		{
			int socketError = WSAGetLastError();
			const char* socketErrorStr = Win32_GetWsaErrorStr(socketError);
			GyLibPrintLine_E("Failed to create socket: %s (%d)", socketErrorStr, socketError);
			//TODO: Can we return some sort of error code? ()
			// Possible Errors: WSANOTINITIALISED, WSAENETDOWN, WSAEAFNOSUPPORT, WSAEINPROGRESS, WSAEMFILE, WSAEINVAL, WSAEINVALIDPROVIDER, WSAEINVALIDPROCTABLE, WSAENOBUFS, WSAEPROTONOSUPPORT, WSAEPROTOTYPE, WSAEPROVIDERFAILEDINIT, WSAESOCKTNOSUPPORT
			return false;
		}
		
		socketOut->isOpen = true;
		
		sockaddr_in win32_address = Win32_GetSockAddrFromIpAddressAndPort(destAddress);
		int connectResult = connect(
			socketOut->handle_win32, //s
			(sockaddr*)&win32_address, //name
			sizeof(win32_address) //namelen
		);
		if (connectResult != 0)
		{
			int socketError = WSAGetLastError();
			const char* socketErrorStr = Win32_GetWsaErrorStr(socketError);
			GyLibPrintLine_E("Failed to connect socket: %s (%d)", socketErrorStr, socketError);
			//TODO: Can we return some sort of error code? ()
			// Possible Errors: WSANOTINITIALISED, WSAENETDOWN, WSAEADDRINUSE, WSAEINTR, WSAEINPROGRESS, WSAEALREADY, WSAEADDRNOTAVAIL, WSAEAFNOSUPPORT, WSAECONNREFUSED, WSAEFAULT, WSAEINVAL, WSAEISCONN, WSAENETUNREACH, WSAEHOSTUNREACH, WSAENOBUFS, WSAENOTSOCK, WSAETIMEDOUT, WSAEWOULDBLOCK, WSAEACCES
			CloseOpenSocket(socketOut);
			return false;
		}
	}
	#else
	#error Unsupported platform for gy_socket.h TryOpenNewSocket
	#endif
	
	return socketOut->isOpen;
}

// +--------------------------------------------------------------+
// |                            Write                             |
// +--------------------------------------------------------------+
bool SocketWriteStr(OpenSocket_t* socket, MyStr_t messageStr)
{
	#if WINDOWS_COMPILATION
	{
		Assert(messageStr.length < INT_MAX);
		int sendResult = send(
			socket->handle_win32, //s
			messageStr.chars,     //buf
			(int)messageStr.length,    //len
			0 //flags
		);
		if (sendResult < 0 || sendResult != (int)messageStr.length)
		{
			int errorCode = WSAGetLastError();
			GyLibPrintLine_E("send failed with error: %s (%d)", Win32_GetWsaErrorStr(errorCode), errorCode);
			//WSANOTINITIALISED, WSAENETDOWN, WSAEACCES, WSAEINTR, WSAEINPROGRESS, WSAEFAULT, WSAENETRESET, WSAENOBUFS, WSAENOTCONN, WSAENOTSOCK, WSAEOPNOTSUPP, WSAESHUTDOWN, WSAEWOULDBLOCK, WSAEMSGSIZE, WSAEHOSTUNREACH, WSAEINVAL, WSAECONNABORTED, WSAECONNRESET, WSAETIMEDOUT
			//TODO: Handle errors!
			return false;
		}
	}
	#else
	
	#endif
	return true;
}

#endif // SOCKETS_SUPPORTED

#endif //  _GY_SOCKET_H
