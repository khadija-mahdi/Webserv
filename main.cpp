#include "Server/Server.hpp"
#include <vector>
#include <signal.h>
#include "Webserv.hpp"
#include "ConfigFile/ParseConfig.hpp"

std::string read_file_content(const std::string &path)
{
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		std::cerr << "Error opening file: " << path << std::endl;
		return "";
	}
	std::ostringstream content;
	content << file.rdbuf();
	file.close();
	return content.str();
}

void HandleSigPip(int signal)
{
	std::string msg = "SigPip " + SSTR(signal) + "\n";
	write(1, msg.c_str(), msg.size());
}

int main(int ac, char **av)
{
	ParseConfig parseConfig;
	std::vector<std::string> taken_ports;
	if (ac != 2)
	{
		DEBUGOUT(1, COLORED("\n Please Provide a Conf file \n", LightGreen));
		exit(1);
	}
	signal(SIGPIPE, HandleSigPip);
	Server server;
	try
	{
		parseConfig.pacingConfigFile(av[1]);

		std::vector<ConfServer> conf_servers = Configurations::http.getConfServes();

		for (size_t i = 0; i < conf_servers.size(); i++)
		{
			if (Containes(taken_ports, conf_servers.at(i).getListen()))
				continue;
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