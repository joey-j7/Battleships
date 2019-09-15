#include "GameObjectProperties.h"
#include "ImGuizmo.h"

#include "Application.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "Transform.h"

void GameObjectProperties::Update()
{
	GameObject* pDebugObject = m_pUIRenderer->GetDebugObject();
	SetActive(pDebugObject);
}

void GameObjectProperties::Render()
{
	GameObject* pDebugObject = m_pUIRenderer->GetDebugObject();
	if (!pDebugObject) return;

	Camera* pCamera = m_pApp->GetSceneManager()->GetActiveScene()->GetCamera();
	glm::mat4 m4Matrix = pDebugObject->GetTransform()->GetTransformMatrix();

	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
  
	if (!pDebugObject->GetTag().empty())
		ImGui::Text(("Properties for " + pDebugObject->GetTag()).c_str());
	else
		ImGui::Text("Properties for Game Object");

	ImGui::Separator();

	std::string sTag = pDebugObject->GetTag();
	sTag.resize(256);

	if (ImGui::InputText("Tag", (char*)sTag.data(), 256))
		pDebugObject->SetTag(sTag);

	ImGui::NewLine();

	if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

	ImGui::SameLine();

	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;

	ImGui::SameLine();

	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	glm::vec3 v3Position = pDebugObject->GetTransform()->GetWorldPosition();
	glm::vec3 v3Rotation = glm::degrees(glm::eulerAngles(pDebugObject->GetTransform()->GetWorldRotation()));
	glm::vec3 v3Scale = pDebugObject->GetTransform()->GetScale();

	ImGui::NewLine();
	if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
		mCurrentGizmoMode = ImGuizmo::LOCAL;
	ImGui::SameLine();
	if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
		mCurrentGizmoMode = ImGuizmo::WORLD;

	if (mCurrentGizmoMode == ImGuizmo::LOCAL)
	{
		v3Position = pDebugObject->GetTransform()->GetPosition();
		v3Rotation = glm::degrees(glm::eulerAngles(pDebugObject->GetTransform()->GetRotation()));
		v3Scale = pDebugObject->GetTransform()->GetScale();
	}

	bool bModified = false;

	ImGui::NewLine();
	if (ImGui::InputFloat3("Translation", &v3Position[0], 3)) bModified = true;
	if (ImGui::InputFloat3("Rotation", &v3Rotation[0], 3)) bModified = true;
	if (ImGui::InputFloat3("Scale", &v3Scale[0], 3)) bModified = true;
	ImGui::NewLine();

	if (bModified)
	{
		m4Matrix = glm::translate(v3Position);
		m4Matrix *= glm::eulerAngleYXZ(v3Rotation.y, v3Rotation.x, v3Rotation.z);
		m4Matrix *= glm::scale(v3Scale);
	}

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate((float*)&pCamera->GetViewMatrix()[0], (float*)&pCamera->GetProjectionMatrix()[0], mCurrentGizmoOperation, mCurrentGizmoMode, (float*)&m4Matrix[0], NULL, NULL);

	/* Only change when modified */
	if (pDebugObject->GetTransform()->GetTransformMatrix() != m4Matrix)
	{
		if (pDebugObject->GetParent())
			m4Matrix = glm::inverse(pDebugObject->GetParent()->GetTransform()->GetTransformMatrix()) * m4Matrix;

		pDebugObject->GetTransform()->SetMatrix(m4Matrix);
		pDebugObject->GetTransform()->SetDirty(true);
	}
}

void GameObjectProperties::SetActive(bool a_bActive)
{
	if (m_bActive == a_bActive)
		return;

	m_bActive = a_bActive;

	if (!m_bActive)
	{
		for (UIElement* pWindow : m_pChildren)
		{
			pWindow->SetActive(m_bActive);
		}

		m_pUIRenderer->SetDebugObject(nullptr);
	}
	else if (m_pParent)
	{
		m_pParent->SetActive(m_bActive);
	}
}
