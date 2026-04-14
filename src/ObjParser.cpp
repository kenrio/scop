#include "ObjParser.hpp"

ObjParser::ObjParser(std::string const &filepath)
{
	parse(filepath);
}

std::vector<float> const &	ObjParser::getVertices(void) const
{
	return (vertices);
}

std::vector<unsigned int> const &	ObjParser::getIndices(void) const
{
	return (indices);
}

Vec3	ObjParser::getCenter(void) const
{
	Vec3	center;

	for (size_t i = 0; i < positions.size(); ++i)
		center = center + positions[i];

	float	n = static_cast<float>(positions.size());

	return (Vec3(center.x / n, center.y / n, center.z / n));
}

void	ObjParser::parse(std::string const &filepath)
{
	std::ifstream	file(filepath);

	if (!file.is_open())
	{
		std::cerr << "Failed to open: " << filepath << std::endl;
		return ;
	}

	std::string	line;

	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
			continue ;

		std::istringstream	iss(line);
		std::string	prefix;

		iss >> prefix;
		if (prefix == "v")
		{
			float	x, y, z;

			iss >> x >> y >> z;
			positions.push_back(Vec3(x, y, z));
		}
		else if (prefix == "f")
			parseFace(line);
	}
	file.close();

	center = getCenter();

	for (size_t i = 0; i < positions.size(); ++i)
		positions[i] = positions[i] - center;

	// for (size_t i = 0; i < positions.size(); ++i)
	// {
	// 	vertices.push_back(positions[i].x);
	// 	vertices.push_back(positions[i].y);
	// 	vertices.push_back(positions[i].z);
	// }

	for (size_t i = 0; i < faces.size(); ++i)
	{
		float	gray = static_cast<float>(i) / faces.size();

		Vec3	v0 = positions[faces[i][0]];
		Vec3	v1 = positions[faces[i][1]];
		Vec3	v2 = positions[faces[i][2]];

		Vec3	normal = Vec3::cross(v1 - v0, v2 - v0).normalize();

		float	ax = std::abs(normal.x);
		float	ay = std::abs(normal.y);
		float	az = std::abs(normal.z);

		for (int j = 0; j < 3; ++j)
		{
			Vec3	&pos = positions[faces[i][j]];

			vertices.push_back(pos.x);
			vertices.push_back(pos.y);
			vertices.push_back(pos.z);
			vertices.push_back(gray);
			vertices.push_back(gray);
			vertices.push_back(gray);

			if (ax >= ay && ax >= az)
			{
				vertices.push_back(pos.z);
				vertices.push_back(pos.y);
			}
			else if (ay >= ax && ay >= az)
			{
				vertices.push_back(pos.z);
				vertices.push_back(pos.x);
			}
			else
			{
				vertices.push_back(pos.y);
				vertices.push_back(pos.x);
			}
		}
	}

	return ;
}

void	ObjParser::parseFace(std::string const &line)
{
	std::istringstream	iss(line);
	std::string			prefix;

	iss >> prefix;

	std::vector<unsigned int>	faceIndices;
	unsigned int				index;

	while (iss >> index)
		faceIndices.push_back(index - 1);

	for (size_t i = 1; i + 1 < faceIndices.size(); ++i)
	{
		indices.push_back(faceIndices[0]);
		indices.push_back(faceIndices[i]);
		indices.push_back(faceIndices[i + 1]);

		std::vector<unsigned int>	triangle = {faceIndices[0], faceIndices[i], faceIndices[i + 1]};
		faces.push_back(triangle);
	}

	return ;
}
