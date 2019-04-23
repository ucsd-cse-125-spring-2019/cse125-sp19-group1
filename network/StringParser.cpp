#include "StringParser.h"

static std::vector<std::pair<std::string, std::string>> parseKeyValueString(const char * data)
{

	std::string dataIn(data);
	std::stringstream strStream(dataIn);
	std::istream &dataStream(strStream);

	std::string clientID = "";
	std::string key_str = "";
	std::string value_str = "";

	while (dataStream.peek() != EOF)
	{
		std::string token_str = "";
		char c = dataStream.peek();

		if (c == '-')
		{
			while (c == '-')
			{
				token_str += dataStream.get();
				c = dataStream.peek();
			}

			// reset client ID
			if (token_str == "-----")
				clientID.clear();
		}

		// Clear key and values at end of line
		if (c == '\n')
		{
			key_str.clear();
			value_str.clear();
		}

		if (isalpha(c) || isdigit(c)) // read word
		{
			while ((isalpha(c) || isdigit(c) || c == '_') || (!key_str.empty() && c != '\n'))
			{
				token_str += dataStream.get();
				c = dataStream.peek();
			}

			if (clientID.empty())
			{
				clientID = token_str;
			}
			else if (key_str.empty())
			{
				key_str = token_str;

			}
			else
			{
				value_str = token_str;

				if (key_str == "location")
				{

					std::vector<float> vec3;
					std::stringstream valStrStream(value_str);
					std::istream &valStream(valStrStream);

					while (valStream.peek() != EOF)
					{
						std::string val_str = "";
						char d = valStream.peek();

						if (isdigit(d) || d == '.')
						{
							while (isdigit(d))
							{
								val_str += valStream.get();
								d = valStream.peek();
							}

							vec3.push_back(std::stof(val_str));
							//std::cout << "num: " << val_str << std::endl;
						}
						else
						{
							d = valStream.get(); // removes whitespace, etc
						}
					}
				}
			}
			//std::cout << "clientID: " << clientID << "  key: " << key_str << "  value: " << value_str << std::endl;
		}
		else // Remove whitespace/miscellanous chars
		{
			c = dataStream.get();
		}


	}

	return std::vector<std::pair<std::string, std::string>>();
}