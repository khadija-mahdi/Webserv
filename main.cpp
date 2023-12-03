#include "Server/Server.hpp"
#include <vector>
#include <signal.h>
#include "Webserv.hpp"

void HandleSigPip(int signal)
{
	std::string msg = "SigPip " + static_cast<std::ostringstream &>(std::ostringstream() << signal).str() + "\n";
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
	 Configurations::Http ;
		std::vector<ConfServer> conf_servers = Configurations::Http::getConfServes();

		for (size_t i = 0; i < conf_servers.size(); i++)
		{
			if (Containes(taken_ports, std::string("3000")))
				continue;
			server.CreatSocket(conf_servers.at(i).getHost(), "3000");
			taken_ports.push_back("3000");
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