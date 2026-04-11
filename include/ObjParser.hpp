#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Vec3.hpp"

class	ObjParser
{

public:

	ObjParser(std::string const &filepath);

	std::vector<float> const		&getVertices(void) const;
	std::vector<unsigned int> const	&getIndices(void) const;

private:

	std::vector<Vec3>			positions;
	std::vector<float>			vertices;
	std::vector<unsigned int>	indices;

	void	parse(std::string const &filepath);
	void	parseFace(std::string const &line);

};
