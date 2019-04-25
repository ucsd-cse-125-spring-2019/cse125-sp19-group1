#include "StringParser.h"

std::vector<std::pair<std::string, std::string>> StringParser::parseKeyValueString(const char * data)
{
	std::vector<std::pair<std::string, std::string>> keyValuePairs;
	std::string dataIn(data);
	std::stringstream strStream(dataIn);
	std::istream &dataStream(strStream);

	std::string key_str = "";
	std::string value_str = "";

	while (dataStream.peek() != EOF)
	{
		std::string token_str = "";
		char c = dataStream.peek();

		// Clear key and values at end of line
		if (c == '\n')
		{
			keyValuePairs.push_back(std::pair<std::string, std::string>(key_str, value_str));
			key_str.clear();
			value_str.clear();
		}

		if (isalpha(c) || isdigit(c) || c == '-') // read word
		{
			while ((isalpha(c) || isdigit(c) || c == '_' || c == '-') || (!key_str.empty() && c != '\n'))
			{
				token_str += dataStream.get();
				c = dataStream.peek();
			}

			if (key_str.empty())
			{
				key_str = token_str;
			}
			else
			{
				value_str = token_str;
			}
		}
		else // Remove whitespace/miscellanous chars
		{
			c = dataStream.get();
		}
	}

	return keyValuePairs;
}