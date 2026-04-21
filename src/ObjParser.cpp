#include "ObjParser.hpp"

ObjParser::ObjParser(std::string const &filePath)
{
	parse(filePath);

	// std::cout << "Positions: " << positions.size() << std::endl;
    // std::cout << "Faces: " << faces.size() << std::endl;
    // std::cout << "Vertices: " << vertices.size() << std::endl;
	// std::cout << "TexCoords: " << texCoords.size() << std::endl;
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

std::vector<float> const &	ObjParser::getNormalLines(void) const
{
	return (normalLines);
}

int	ObjParser::getPositionCount(void) const
{
	return (positions.size());
}

int	ObjParser::getFaceCount(void) const
{
	return (faces.size() / 3);
}

bool	ObjParser::isValid(void) const
{
	return (!positions.empty() && !faces.empty());
}

void	ObjParser::parse(std::string const &filePath)
{
	parseFile(filePath);

	if (positions.empty())
	{
		std::cerr << "No vertices found in: " << filePath << std::endl;
		return ;
	}

	center = getCenter();
	for (size_t i = 0; i < positions.size(); ++i)
		positions[i] = positions[i] - center;

	if (!faces.empty())
		buildVertices();

	return ;
}

void	ObjParser::parseFile(const std::string &filePath)
{
	std::ifstream	file(filePath);

	if (!file.is_open())
	{
		std::cerr << "Failed to open: " << filePath << std::endl;
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
		else if (prefix == "vt")
		{
			float	u, v;
			iss >> u >> v;
			texCoords.push_back(Vec3(u, v, 0.0f));
		}
		else if (prefix == "f")
			parseFace(line);
	}
	file.close();

	return ;
}

void	ObjParser::buildVertices(void)
{
	float	minZ = positions[0].z, maxZ = positions[0].z;
	float	minY = positions[0].y, maxY = positions[0].y;

	for (size_t i = 1; i < positions.size(); ++i)
	{
		if (positions[i].z < minZ) minZ = positions[i].z;
		if (positions[i].z > maxZ) maxZ = positions[i].z;
		if (positions[i].y < minY) minY = positions[i].y;
		if (positions[i].y > maxY) maxY = positions[i].y;
	}

	for (size_t i = 0; i < faces.size(); i += 3)
	{
		int		triangleIndex = i / 3;
		float	gray = faceColors[triangleIndex % 3];

		Vec3	&posA = positions[faces[i].v];
		Vec3	&posB = positions[faces[i + 1].v];
		Vec3	&posC = positions[faces[i + 2].v];

		bool	hasAllNormals =
				faces[i].vn >= 0 &&
				faces[i + 1].vn >= 0 &&
				faces[i + 2].vn >= 0;

		Vec3	faceNormal;
		if (!hasAllNormals)
			faceNormal = Vec3::cross(posB - posA, posC - posA).normalize();

		Vec3	faceCenter(
							(posA.x + posB.x + posC.x) / 3.0f,
							(posA.y + posB.y + posC.y) / 3.0f,
							(posA.z + posB.z + posC.z) / 3.0f);

		Vec3	displayNormal = faceNormal;
		if (hasAllNormals)
		{
			displayNormal = (
							normals[faces[i].vn] +
							normals[faces[i + 1].vn] +
							normals[faces[i + 2].vn]) * (1.0f / 3.0f);
			displayNormal = displayNormal.normalize();
		}

		normalLines.push_back(faceCenter.x);
		normalLines.push_back(faceCenter.y);
		normalLines.push_back(faceCenter.z);

		normalLines.push_back(faceCenter.x + displayNormal.x * NORMAL_LENGTH);
		normalLines.push_back(faceCenter.y + displayNormal.y * NORMAL_LENGTH);
		normalLines.push_back(faceCenter.z + displayNormal.z * NORMAL_LENGTH);

		for (int j = 0; j < 3; ++j)
		{
			FaceVertex	&fv = faces[i + j];
			Vec3		&pos = positions[fv.v];

			Vec3	normal = (hasAllNormals) ? normals[fv.vn] : faceNormal;

			Vec3	uv(0.0f, 0.0f, 0.0f);
			if (fv.vt >= 0 && fv.vt < (int)texCoords.size())
				uv = texCoords[fv.vt];

			vertices.push_back(pos.x);
			vertices.push_back(pos.y);
			vertices.push_back(pos.z);

			vertices.push_back(gray);
			vertices.push_back(gray);
			vertices.push_back(gray);

			vertices.push_back((pos.z - minZ) / (maxZ - minZ));
			vertices.push_back((pos.y - minY) / (maxY - minY));

			vertices.push_back(uv.x);
			vertices.push_back(uv.y);

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
