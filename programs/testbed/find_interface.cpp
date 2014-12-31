#include "configCosmos.h"
#include "agentlib.h"

int main (int argc, const char* argv[])
{

	vector<agent_channel> interfaces;

	interfaces = agent_find_addresses(AGENT_TYPE_UDP);

	for (uint16_t i=0; i<interfaces.size(); ++i)
	{
		printf("%d. %s : %s\n", i, interfaces[i].name, interfaces[i].address);
		//			inet_ntop(ifr->ifr_addr.sa_family, &((struct sockaddr_in*)&ifr->ifr_netmask)->sin_addr, addrbuf, sizeof(addrbuf)));
	}
}
