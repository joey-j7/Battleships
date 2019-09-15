#pragma once

#include "ComponentSystem.h"
#include <map>
#include <unordered_map>
#include "Renderer.h"

class Mesh;
class Camera;
class PhysicsDebugDrawer;

class Model;
class VBO;
class IBO;

class RenderSystem : public ComponentSystem
{
public:
	RenderSystem();
	~RenderSystem();

	typedef std::map<
		Component::Type, std::unordered_map<
			VBO*, std::unordered_map<
				ShaderProgram*, std::unordered_map<
					IBO*, std::unordered_map<
						Texture*, std::vector <
							Renderer*
						>
					>
				>
			>
		>
	> SortedRenderers;

	static GameObject* LightObject;

protected:
	bool CanProcessComponent(Component* a_pComponent) override;
	void ProcessComponents() override;

	void OnComponentDestroyed(Component* a_pComponent) override;
	void OnComponentAdded(Component* a_pComponent) override;

private:
	Camera* m_pCamera = nullptr;
	PhysicsDebugDrawer* m_pPhysicsDebugDrawer = nullptr;

	SortedRenderers m_vRenderBatches;
};
