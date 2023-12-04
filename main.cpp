#include "Server/Server.hpp"
#include <vector>
#include <signal.h>
#include "Webserv.hpp"

void HandleSigPip(int signal)
{
	std::string msg = "SigPip " + SSRT(signal) + "\n";
	write(1, msg.c_str(), msg.size());
}

int main(int ac, char **av)
{

	std::vector<std::string> taken_ports;
	if (ac != 2)
	{
		std::cout << "Please Provide a Conf file" << std::endl;
		exit(1);
	}
	signal(SIGPIPE, HandleSigPip);
	Server server;
	try
	{
		pacingConfigFile();
		DEBUGOUT(1, COLORED("Max Body Size : " << Configurations::http.getMax_body_size(), Green));

		std::vector<ConfServer> conf_servers = Configurations::http.getConfServes();

		for (size_t i = 0; i < conf_servers.size(); i++)
		{
			DEBUGOUT(1,COLORED("port is : " << conf_servers.at(i).getListen(), Red));
			if (Containes(taken_ports, conf_servers.at(i).getListen()))
				continue;
			DEBUGOUT(1, COLORED("getHost : " << conf_servers.at(i).getHost(), Green));
			server.CreatSocket(conf_servers.at(i).getHost(), conf_servers.at(i).getListen());
			taken_ports.push_back(conf_servers.at(i).getListen());
		}
		DEBUGOUT(1, COLORED("Running Safe And Well", Green));
		server.Run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}
	return (0);
}