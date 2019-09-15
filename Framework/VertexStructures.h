#pragma once

#include <glm/glm.hpp>
#include "imgui.h"

struct VertexData
{
	enum Type
	{
		E_DEFAULT,
		E_PARTICLE,
		E_IMGUI
	};

	static size_t GetSize(Type a_eType)
	{
		if (a_eType == E_IMGUI)
			return sizeof(ImDrawVert);
		else if (a_eType == E_PARTICLE)
			return sizeof(Particle);
		
		return sizeof(Default);
	}

	static std::vector<int> GetOffsets(Type a_eType)
	{
		if (a_eType == E_IMGUI)
			return { 0, sizeof(ImVec2), sizeof(ImVec2) * 2 };
		else if (a_eType == E_PARTICLE)
			return { 0, sizeof(float), sizeof(float) * 2, sizeof(float) * 3 };
		
		return { 0, sizeof(glm::vec3), sizeof(glm::vec3) * 2 };
	}

	struct Default
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 UV;
	};

	struct Particle
	{
		float ID;
		float RadiusOffset;
		float VelocityOffset;
		float SizeOffset;
	};

	struct ImDrawVert
	{
		ImVec2  Position;
		ImVec2  UV;
		ImU32   Color;
	};
};
