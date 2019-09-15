#include "GameObjectList.h"

#include "Application.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Scene.h"

using namespace UIElements;

void GameObjectList::Render()
{
	const std::vector<GameObject*>& pObjects = m_pApp->GetSceneManager()->GetActiveScene()->GetGameObjects();

	for (GLuint i = 0; i < pObjects.size(); i++)
	{
		if (pObjects[i]->GetParent())
			continue;

		ShowObjectItem(pObjects[i], i);
	}
}

void GameObjectList::ShowObjectItem(GameObject* a_pObject, int a_iIndex)
{
	/* Create name */
	std::string sTag = a_pObject->GetTag();

	if (sTag.empty())
		sTag = "Game Object";

	std::string sName = "[" + to_string(a_iIndex) + "] " + sTag;
	const size_t iCount = a_pObject->GetChildren().size();

	bool bOpen = ImGui::TreeNodeEx(sName.c_str(), iCount == 0 ? ImGuiTreeNodeFlags_Bullet : ImGuiTreeNodeFlags_OpenOnArrow);

	if (ImGui::IsItemClicked()) {
		m_pUIRenderer->SetDebugObject(a_pObject);
	}

	if (bOpen)
	{
		for (GLuint i = 0; i < a_pObject->GetChildren().size(); i++)
		{
			ShowObjectItem(a_pObject->GetChildren()[i], i);
		}

		ImGui::TreePop();
	}
}
