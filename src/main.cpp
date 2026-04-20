#include <iostream>
#include <string>

#include "ModelViewer.hpp"

int	main(int argc, char *argv[])
{
	std::string	objPath;
	if (argc >= 2)
		objPath = argv[1];
	else
		objPath = DEFAULT_OBJ_PATH;
	
	ModelViewer	modelViewer(objPath);
	modelViewer.run();

	return (0);
}	
