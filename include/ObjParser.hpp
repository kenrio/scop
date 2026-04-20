# ifndef OBJPARSER_HPP
# define OBJPARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "math/Vec3.hpp"

struct	FaceVertex
{
	int	v;
	int	vt;
	int	vn;
};

class	ObjParser
{

public:

	ObjParser(std::string const &filePath);

	std::vector<float> const		&getVertices(void) const;
	std::vector<unsigned int> const	&getIndices(void) const;
	Vec3							getCenter(void) const;

private:

	std::vector<Vec3>			positions;
	std::vector<Vec3>			normals;
	std::vector<Vec3>			texCoords;
	std::vector<FaceVertex>		faces;
	std::vector<float>			vertices;
	std::vector<unsigned int>	indices;
	Vec3						center;

	void	parse(std::string const &filePath);
	void	parseFile(const std::string &filePath);
	void	buildVertices(void);

	void	parseFace(std::string const &line);
	void	parseVertexToken(std::string const &token, int &vIdx, int &vtIdx, int &vnIdx);

};

#endif
