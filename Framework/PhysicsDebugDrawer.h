#pragma once
#include <bullet/include/LinearMath/btIDebugDraw.h>
#include <glm/glm.hpp>

struct DebugVertex
{
	glm::vec3 Position;
	glm::vec3 Color;
};

class ShaderProgram;
class PhysicsDebugDrawer : public btIDebugDraw
{
public:
	static const int MAX_DEBUG_LINES = 16384;

	static PhysicsDebugDrawer* GetInstance()
	{
		if (m_pInstance == nullptr)
			m_pInstance = new PhysicsDebugDrawer();

		return m_pInstance;
	}

	void Draw(glm::mat4& a_m4ViewProj);
	unsigned int GetLineCount() const { return m_uiLineCount; }

	void drawLine(const btVector3& a_v3From, const btVector3& a_v3To, const btVector3& a_v3Color) override;
	void drawContactPoint(const btVector3& a_v3PointOnB, const btVector3& a_v3NormalOnB, btScalar a_sDistance, int a_iLifeTime, const btVector3& a_v3Color) override {}
	void reportErrorWarning(const char* a_pcWarningString) override;
	void draw3dText(const btVector3& a_v3Location, const char* a_pcTextString) override {}
	void setDebugMode(int a_iDebugMode) override { m_iDebugMode = a_iDebugMode; }
	int	getDebugMode() const override { return m_iDebugMode; }

	void SetDebugDraw(bool a_bDrawDebug)
	{
		m_bDrawDebug = a_bDrawDebug;
	}

	bool DoesDrawDebug() const
	{
		return m_bDrawDebug;
	}

private:
	PhysicsDebugDrawer();

	ShaderProgram* m_pShader = nullptr;

	unsigned int m_uiLineCount;
	DebugVertex m_aVertexBuffer[MAX_DEBUG_LINES * 2];

	int m_iDebugMode;
	bool m_bDrawDebug = false;

	static PhysicsDebugDrawer* m_pInstance;
};
