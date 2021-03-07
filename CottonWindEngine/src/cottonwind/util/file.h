#pragma once

#include <cstdio>
#include <malloc.h>
#include <string>

namespace cotwin
{
	/*std::vector<std::string> split(std::string& line, char delimiter)
	{
		std::vector<std::string> tokens;
		std::istringstream token_stream(line);
		std::string token;
		while (std::getline(token_stream, token, delimiter))
		{
			tokens.push_back(token);
		}
		return tokens;
	}*/

	std::string read_file(const char* filepath)
	{
		FILE* file;
		fopen_s(&file, filepath, "rt");			// rt stands for 'read text'
		fseek(file, 0, SEEK_END);				// go to the end of file
		unsigned long length = ftell(file);		// tell the amount of passed bytes
		char* data = (char*)_malloca(length + 1);	// +1 for the terminator, allocate on the stack
		memset(data, 0, length + 1);
		fseek(file, 0, SEEK_SET);				// go to the beggining of the file
		fread(data, 1, length, file);
		fclose(file);

		std::string result(data);

		return result;
	}
}