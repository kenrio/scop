#include "ObjParser.hpp"

ObjParser::ObjParser(std::string const &filepath)
{
	parse(filepath);
	std::cout << "Positions: " << positions.size() << std::endl;
    std::cout << "Faces: " << faces.size() << std::endl;
    std::cout << "Vertices: " << vertices.size() << std::endl;
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
		else if (prefix == "vn")
		{
			float	x, y, z;
			iss >> x >> y >> z;
			normals.push_back(Vec3(x, y, z));
		}
		else if (prefix == "f")
			parseFace(line);
	}
	file.close();

	center = getCenter();

	for (size_t i = 0; i < positions.size(); ++i)
		positions[i] = positions[i] - center;

	for (size_t i = 0; i < faces.size(); i += 3)
	{
		float	gray = static_cast<float>(i / 3) / (faces.size() / 3);

		bool	hasAllNormals =
				faces[i].vn >= 0 &&
				faces[i + 1].vn >= 0 &&
				faces[i + 2].vn >= 0;

		Vec3	faceNormal;
		if (!hasAllNormals)
		{
			Vec3	&posA = positions[faces[i].v];
			Vec3	&posB = positions[faces[i + 1].v];
			Vec3	&posC = positions[faces[i + 2].v];
			faceNormal = Vec3::cross(posB - posA, posC - posA).normalize();
		}


		for (int j = 0; j < 3; ++j)
		{
			FaceVertex	&fv = faces[i + j];
			Vec3		&pos = positions[fv.v];

			Vec3	normal = (hasAllNormals) ? normals[fv.vn] : faceNormal;

			vertices.push_back(pos.x);
			vertices.push_back(pos.y);
			vertices.push_back(pos.z);

			vertices.push_back(gray);
			vertices.push_back(gray);
			vertices.push_back(gray);

			vertices.push_back(pos.z);
			vertices.push_back(pos.y);

			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);
		}
	}

	return ;
}

void	ObjParser::parseFace(std::string const &line)
{
	std::istringstream	iss(line);
	std::string			prefix;

	iss >> prefix;

	std::vector<FaceVertex>		faceVertices;
	std::string					token;
	
	while (iss >> token)
	{
		FaceVertex	fv;
		parseVertexToken(token, fv.v, fv.vt, fv.vn);
		faceVertices.push_back(fv);
	}

	// fan triangulation（扇形三角形分割）
	for (size_t i = 1; i + 1 < faceVertices.size(); ++i)
	{
		faces.push_back(faceVertices[0]);
		faces.push_back(faceVertices[i]);
		faces.push_back(faceVertices[i + 1]);

		indices.push_back(faceVertices[0].v);
		indices.push_back(faceVertices[i].v);
		indices.push_back(faceVertices[i + 1].v);
	}

	return ;
}

void	ObjParser::parseVertexToken(std::string const &token, int &vIdx, int &vtIdx, int &vnIdx)
{
	vIdx = -1;
	vtIdx = -1;
	vnIdx = -1;

	std::istringstream	iss(token);
	std::string			part;

	if (std::getline(iss, part, '/') && !part.empty())
		vIdx = std::stoi(part) - 1;

	if (std::getline(iss, part, '/') && !part.empty())
		vtIdx = std::stoi(part) - 1;

	if (std::getline(iss, part, '/') && !part.empty())
		vnIdx = std::stoi(part) - 1;

	return ;
}
