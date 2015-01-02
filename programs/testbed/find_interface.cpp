#include "configCosmos.h"
#include "socketlib.h"

int main (int argc, const char* argv[])
{

	vector<socket_channel> interfaces;

	interfaces = socket_find_addresses(SOCKET_TYPE_UDP);

	for (uint16_t i=0; i<interfaces.size(); ++i)
	{
		printf("%d. %s : %s\n", i, interfaces[i].name, interfaces[i].address);
		//			inet_ntop(ifr->ifr_addr.sa_family, &((struct sockaddr_in*)&ifr->ifr_netmask)->sin_addr, addrbuf, sizeof(addrbuf)));
	}
}
