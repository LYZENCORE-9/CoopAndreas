#if !defined(_COOPSERVER_SERVER_CLI_COMMANDS_SERVERCOMMAND_H_) || !defined(_COOPSERVER_SERVERCOMMAND_H_)
#define _COOPSERVER_SERVER_CLI_COMMANDS_SERVERCOMMAND_H_
#define _COOPSERVER_SERVERCOMMAND_H_

#include <iostream>
#include <cstdio>
#include <cstring>

class ServerCommand
{
public:
	static int HelpCommand(char *s0, char *s1, char *s2);
};

#endif