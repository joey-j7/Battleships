#include "Shape.h"

#include "VBO.h"
#include "IBO.h"

#include "VertexStructures.h"

Shape::~Shape()
{
	delete m_pVBO;
	delete m_pIBO;
}

void Shape::Load(Type a_eType, unsigned a_uiSegments)
{
	m_uiSegments = glm::max(a_uiSegments, (unsigned)1);
	if (a_eType == E_NONE) LoadEmpty();
	else if (a_eType == E_QUAD) LoadQuad();

	m_bIsLoaded = true;
}

void Shape::LoadQuad(bool a_bIsVertical)
{
	const unsigned int uiVertexCount = (m_uiSegments + 1) * (m_uiSegments + 1);
	const unsigned int uiIndexCount = m_uiSegments * m_uiSegments * 6;
	
	std::vector<VertexData::Default> veVertices;
	veVertices.resize(uiVertexCount);
  
	std::vector<GLushort> veIndices;
	veIndices.resize(uiIndexCount);
  
	const float fInvDetail = 1.0f / m_uiSegments;
	unsigned int fQuads = 0;
	unsigned int fOffset = 0;

	const glm::vec3 v3Up = a_bIsVertical ? glm::vec3(0, 1, 0) : glm::vec3(0, 0, -1);
	const glm::vec3 v3Forward = a_bIsVertical ? glm::vec3(0, 0, -1) : glm::vec3(0, 1, 0);

	const glm::vec2 v2UVs[4]
	{
		glm::vec2(0, 1),
		glm::vec2(1, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 0)
	};

	for (int y = 0; y < m_uiSegments + 1; ++y)
	{
		for (int x = 0; x < m_uiSegments + 1; ++x)
		{
			const int i = (y * (1 + m_uiSegments) + x);

			veVertices[i].Position = (glm::vec3(x, 0, 0) + (float)y * v3Up) * fInvDetail;
			veVertices[i].Position -= (glm::vec3(0.5f, 0, 0) + v3Up * 0.5f);
			veVertices[i].UV = v2UVs[i % 4] * fInvDetail;
			veVertices[i].Normal = v3Forward;

			if (x < m_uiSegments && y < m_uiSegments)
			{
				veIndices[fQuads * 6 + 0] = fQuads + fOffset + 0;
				veIndices[fQuads * 6 + 1] = fQuads + fOffset + (m_uiSegments + 1) + 1;
				veIndices[fQuads * 6 + 2] = fQuads + fOffset + (m_uiSegments + 1) + 0;

				veIndices[fQuads * 6 + 3] = fQuads + fOffset + 0;
				veIndices[fQuads * 6 + 4] = fQuads + fOffset + 1;
				veIndices[fQuads * 6 + 5] = fQuads + fOffset + (m_uiSegments + 1) + 1;

				fQuads++;
			}
			else fOffset++;
		}
	}
  
	m_pVBO = new VBO(uiVertexCount, VertexData::Type::E_DEFAULT, veVertices.data());
	m_pIBO = new IBO(uiIndexCount, sizeof(GLushort), veIndices.data());

	m_Type = E_QUAD;
}

void Shape::LoadEmpty()
{
	m_pVBO = new VBO();
	m_pIBO = new IBO();

	m_Type = E_NONE;
}


void Shape::CalculateBounds()
{
	m_v3BoundsMin = m_pIBO->GetBoundsMin();
	m_v3BoundsMax = m_pIBO->GetBoundsMax();

	m_v3Extents = (m_v3BoundsMax - m_v3BoundsMin) * 0.5f;
	m_v3Offset = (m_v3BoundsMax + m_v3BoundsMin) * 0.5f;

	m_fRadius = glm::distance(glm::vec3(), m_v3Extents);
}
