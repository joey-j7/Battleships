#pragma once

#include <string>
#include <vector>

#include <unordered_map>

#include "GLIncludes.h"

#include "ReferencedObject.h"

#include <glm/glm.hpp>
#include "Mesh.h"

#include "Utils.h"

class VBO;
class IBO;
class Texture;
class GameObject;

class Model : public ReferencedObject
{
public:
	enum Type
	{
		E_NONE = 0,
		E_OBJ  = 1,
		E_MD2  = 2
	};

	Model() = default;
	virtual ~Model();

	virtual void Load(const std::string& a_sFilePath) = 0;

	VBO* GetVBO() const
	{
		return m_pVBO;
	}

	IBO* GetIBO(const GLuint& a_uiID) const;
	const std::vector<Texture*>& GetTextures(const GLuint& a_uiID) const;

	const std::unordered_map<std::string, Mesh::Animation>& GetAnimations(const GLuint& a_uiID) const;

	const std::vector <Mesh*>& GetMeshes() const
	{
		return m_vMeshes;
	}

	const std::string& GetPath() const
	{
		return m_sFilePath;
	}

	const Type& GetType() const
	{
		return m_Type;
	}

	GLuint GetVertexCount() const
	{
		return m_uiVertexCount;
	}
  
	const glm::vec3& GetExtents() const
	{
		return m_v3Extents;
	}
  
	void GetBounds(glm::vec3& a_v3Min, glm::vec3& a_v3Max) const
	{
		a_v3Min = m_v3BoundsMin;
		a_v3Max = m_v3BoundsMax;
	}

	const glm::vec3& GetOffset() const
	{
		return m_v3Offset;
	}

	static std::pair<GameObject*, Model*> CreateObject(
		const std::string& a_sPath,
		const std::string& a_sVertexPath = "Assets/Engine/Shaders/Default/Specular.vsh",
		const std::string& a_sFragmentPath = "Assets/Engine/Shaders/Default/Specular.fsh"
	);

protected:
	void CalculateBounds();

	std::string m_sFilePath = "";

	VBO* m_pVBO = nullptr;
	GLuint m_uiVertexCount = 0;

	std::vector <Mesh*> m_vMeshes;
	Type m_Type = E_NONE;
  
	/* Bounds */
	glm::vec3 m_v3BoundsMin;
	glm::vec3 m_v3BoundsMax;

	glm::vec3 m_v3Extents;
	glm::vec3 m_v3Offset;

	float m_fRadius;
};
