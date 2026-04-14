#include <iostream>
#include <string>

#include "ModelViewer.hpp"

int	main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: ./scop <obj_file_path>" << std::endl;
		return (1);
	}

	std::string	objPath = argv[1];
	ModelViewer	modelViewer(objPath);

	modelViewer.run();

	return (0);
}	
