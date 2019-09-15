#pragma once

#include <string>
#include <unordered_map>

#include <vector>

class Application;
class IBO;
class Texture;

class Mesh
{
public:
	struct Animation
	{
		unsigned int StartFrame;
		unsigned int EndFrame;
		unsigned int FPS = 15;
	};

	Mesh(
		unsigned int a_uiIdentity,
		IBO* a_pIBO,
		const std::unordered_map<std::string, Animation>& a_mAnimations,
		const std::vector<Texture*>& a_vTextures
	)
	{
		m_uiIdentity = a_uiIdentity;
		m_pIBO = a_pIBO;
		m_mAnimations = a_mAnimations;
		m_vTextures = a_vTextures;
	}

	virtual ~Mesh();

	unsigned int GetID() const
	{
		return m_uiIdentity;
	}

	IBO* GetIBO() const
	{
		return m_pIBO;
	}

	const std::vector<Texture*>& GetTextures() const
	{
		return m_vTextures;
	}

  const std::unordered_map<std::string, Animation>& GetAnimations() const
	{
		return m_mAnimations;
	}

private:
	unsigned int m_uiIdentity = 0;
	IBO* m_pIBO = nullptr;

	std::vector<Texture*> m_vTextures;
	std::unordered_map <std::string, Animation> m_mAnimations;
};