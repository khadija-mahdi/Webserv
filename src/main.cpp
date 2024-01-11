#include "Server/Server.hpp"
#include <vector>
#include <signal.h>
#include "ConfigFile/ParseConfig.hpp"
#include "Core/Include/Core.hpp"

void HandleSigPip(int signal)
{
	std::string msg = "SigPip " + SSTR(signal) + "\n";
	write(1, msg.c_str(), msg.size());
}

bool FindTakenPorts(ServerMap &map, std::string port)
{
	ServerMap::iterator it;
	it = map.find(port);
	if (it != map.end())
		return (true);
	return (false);
}

bool IsServerNamesEquals(std::vector<std::string> s1, std::vector<std::string> s2)
{
	if (s1.size() != s2.size())
		return false;
	for (size_t i = 0; i < s1.size(); i++)
	{
		if (s1.at(i) != s2.at(i))
			return (false);
	}
	return (true);
}

int main(int ac, char **av)
{
	(void)ac;

	ServerMap taken_ports;
	std::vector<ConfServer> conf_servers;
	Server server;
	ParseConfig parseConfig;
	signal(SIGPIPE, HandleSigPip);

	try
	{
		parseConfig.pacingConfigFile((char *)(ac == 2 ? av[1] : "config/config_file.conf"));
		conf_servers = Configurations::http.getConfServes();

		for (size_t i = 0; i < conf_servers.size(); i++)
		{
			if (FindTakenPorts(taken_ports, conf_servers.at(i).getListen()) &&
				!IsServerNamesEquals(taken_ports[conf_servers.at(i).getListen()],
									 conf_servers.at(i).getConfServer_names()))
				continue;
			server.CreatSocket(conf_servers.at(i).getHost(), conf_servers.at(i).getListen());
			taken_ports[conf_servers.at(i).getListen()] = conf_servers.at(i).getConfServer_names();
		}
		server.Run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}
	return (0);
}