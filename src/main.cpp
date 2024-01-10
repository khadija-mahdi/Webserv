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

int main(int ac, char **av)
{
	std::vector<std::string> taken_ports;
	std::vector<ConfServer> conf_servers;
	Server server;
	ParseConfig parseConfig;

	signal(SIGPIPE, HandleSigPip);
	if (ac != 2)
	{
		std::cout << "Please Provide a Conf file" << std::endl;
		exit(1);
	}

	try
	{

		parseConfig.pacingConfigFile(av[1]);
		conf_servers = Configurations::http.getConfServes();

		for (size_t i = 0; i < conf_servers.size(); i++)
		{
			// if (Containes(taken_ports, conf_servers.at(i).getListen()))
			// 	continue;
			server.CreatSocket(conf_servers.at(i).getHost(), conf_servers.at(i).getListen());
			taken_ports.push_back(conf_servers.at(i).getListen());
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