#include "Configurations.hpp"
#define MB 1048576
#define GB 1073741824


//---------------------------------Events class -------------------------------
int Configurations::Events::worker_connections = 0;
Http Configurations::http;

int Configurations::Events::getWorkerConnections() { return worker_connections; }

void Configurations::Events::setWorkerConnections(int worCon) { worker_connections = worCon; }

//------------------------------------------Http class --------------------------------------------

unsigned long long Http::getMax_body_size() { return ConvertToBytes(max_body_size); }
std::string Http::getDefault_type() { return default_type; }
std::map<int, std::string> Http::getError_pages() { return error_pages; }
std::map<std::string, std::string> Http::getMimeTypes() const { return MimeTypes; }
std::vector<ConfServer> Http::getConfServes() { return ConfServers; }
void Http::setMax_body_size(std::string &bodySize) { max_body_size = bodySize; }
void Http::setDefault_type(std::string &defTyp) { default_type = defTyp; }

void Http::setError_pages(std::map<int, std::string> &maperr) { error_pages = maperr; }
void Http::setMimeTypes(std::string const &_value, std::string const &_key) { MimeTypes[_key] = _value; }
void Http::SetReverseMimeTypes(std::string const &_value, std::string const &_key) { ReverseMimeTypes[_key] = _value; }
void Http::addServer(ConfServer const &loc) { ConfServers.push_back(loc); }

std::string Http::GetFileExtention(std::string ContentType)
{
	return this->ReverseMimeTypes[ContentType];
}
std::map<std::string, std::string> ReverseTypesMap(std::map<std::string, std::string> m)
{
	std::map<std::string, std::string> r;
	for (std::map<std::string, std::string>::iterator i = m.begin(); i != m.end(); i++)
		r[i->second] = i->first;
	return r;
}

unsigned long long ConvertToBytes(std::string &value)
{
	std::stringstream errorMessage;

	char MUnit;
	unsigned long long DecimalValue;

	for (size_t i = 0; i < value.size() - 1; i++)
	{
		if (!isdigit(value[i]))
		{
			errorMessage << "\033[1;" << 31 << "mError: "
						<< "\nInvalide Value of `client_max_body_size` Incorrect" << value
						<< "\033[0m" << std::endl;
			throw std::runtime_error(errorMessage.str());
		}
	}
	DecimalValue = atoll(value.c_str());
	MUnit = *(value.end() - 1);
	MUnit = tolower(MUnit);
	switch (MUnit)
	{
	case 'b':
		// do nothing its already in bytes
		break;
	case 'k':
		DecimalValue *= 1024;
		break;
	case 'm':
		DecimalValue *= MB;
		break;
	case 'g':
		DecimalValue *= GB;
		break;
	default:
		throw std::runtime_error("Invalide measure unit " + std::string(1, MUnit));
		break;
	}
	return (DecimalValue);
}

//------------------------------------------Values class --------------------------------------------

void checkValues(const std::string &lines)
{
	std::stringstream errorMessage;
	std::istringstream lineStream(lines);
	std::string line;
	size_t nextLine = 0;

	while (std::getline(lineStream, line))
	{

		if (nextLine == 1)
		{
			size_t openBrace = line.find('{');
			size_t closeBrace = line.find('}');

			if (openBrace == std::string::npos && closeBrace == std::string::npos)
			{
				errorMessage << "\033[1;31mError: Syntax error. Please add The Value inside Braces "
							 << "\033[0m" << std::endl;
				throw std::runtime_error(errorMessage.str());
			}

			nextLine = 0;
		}

		size_t openBrace = line.find('}');
		if (openBrace != std::string::npos)
			nextLine = 1;
	}
}

std::vector<std::pair<std::string, std::pair<int, int> > > &Values::BlocksS(const std::string &lines, const std::string &blockName)
{
	size_t pos = 0;
	checkValues(lines);
	while (pos < lines.length())
	{
		size_t start = lines.find(blockName, pos);
		if (start == std::string::npos)
			break;
		size_t openBrace = lines.find('{', start);
		if (openBrace == std::string::npos)
			break;
		if (blockName == "location")
		{
			ConfServer server;
			std::string path = lines.substr(start + 9, openBrace - (start + 10));
			paths.push_back(path);
		}
		size_t closeBrace = openBrace + 1;
		size_t bracesCount = 1;
		while (bracesCount > 0 && closeBrace < lines.length())
		{
			if (lines[closeBrace] == '{')
			{
				bracesCount++;
			}
			else if (lines[closeBrace] == '}')
				bracesCount--;
			closeBrace++;
		}
		if (bracesCount == 0)
		{
			std::string block = lines.substr(openBrace, closeBrace - openBrace);
			extractedBlocks.push_back(std::make_pair(block, std::make_pair(start, closeBrace)));
			pos = closeBrace;
		}
		else
			break;
	}
	return extractedBlocks;
}

std::vector<std::string> Values::getPaths() const { return paths; }
void Values::setPaths(std::string const &nPAThs) { paths.push_back(nPAThs); }
