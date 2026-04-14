# ifndef OBJPARSER_HPP
# define OBJPARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "math/Vec3.hpp"

class	ObjParser
{

public:

	ObjParser(std::string const &filepath);

	std::vector<float> const		&getVertices(void) const;
	std::vector<unsigned int> const	&getIndices(void) const;
	Vec3							getCenter(void) const;

private:

	std::vector<Vec3>						positions;
	std::vector<float>						vertices;
	std::vector<std::vector<unsigned int>>	faces;
	std::vector<unsigned int>				indices;
	Vec3									center;

	void	parse(std::string const &filepath);
	void	parseFace(std::string const &line);

};

#endif
