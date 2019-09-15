#pragma once

#include "ReferencedObject.h"

#include <glm/glm.hpp>
#include "Mesh.h"

class VBO;
class IBO;
class Texture;

class Shape : public ReferencedObject
{
public:
	enum Type
	{
		E_NONE = 0,
		E_QUAD = 1
	};

	virtual ~Shape();

	void Load(Type a_eType, unsigned int a_uiSegments);

	VBO* GetVBO() const
	{
		return m_pVBO;
	}
  
	IBO* GetIBO() const
	{
		return m_pIBO;
	}
  
	unsigned int GetSegmentCount() const
	{
		return m_uiSegments;
	}

	const Type& GetType() const
	{
		return m_Type;
	}

	const glm::vec3& GetExtents() const
	{
		return m_v3Extents;
	}

	const glm::vec3& GetOffset() const
	{
		return m_v3Offset;
	}

protected:
	void LoadQuad(bool a_bIsVertical = false);
	void LoadEmpty();
	
	void CalculateBounds();

	VBO* m_pVBO = nullptr;
	IBO* m_pIBO = nullptr;

	Type m_Type = E_NONE;
	unsigned int m_uiSegments;
  
	/* Bounds */
	glm::vec3 m_v3BoundsMin;
	glm::vec3 m_v3BoundsMax;

	glm::vec3 m_v3Extents;
	glm::vec3 m_v3Offset;

	float m_fRadius;
};
