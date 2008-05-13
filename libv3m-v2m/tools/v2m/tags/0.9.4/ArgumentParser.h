#ifndef __ARGUMENTPARSER_H
#define __ARGUMENTPARSER_H

using namespace std;

class ArgumentParser {

public:
	ArgumentParser (int argc, char **argv);
	~ArgumentParser ();

private:
	void printUsage ();
	int argumentCheck(std::string arg);
};

#endif
