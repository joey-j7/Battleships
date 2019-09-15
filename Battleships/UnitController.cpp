#include "UnitController.h"

#include "Application.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "InputManager.h"

#include "BaseShip.h"
#include "GameObject.h"

#include "NetworkIdentity.h"
#include "HPBar.h"

#include "MeshRenderer.h"
#include "GameTime.h"
#include "ParticleRenderer.h"

UnitController::UnitController(GameObject* a_pObj)
	: NetworkBehavior(a_pObj)
{
	m_pSelectionQuad = GameObject::Create();

	MeshRenderer* pMeshRenderer = new MeshRenderer(m_pSelectionQuad, Shape::E_QUAD, 1, Application::GetInstance()->GetLoadInterface()->LoadTexture("Assets/Game/Textures/UI/HUD/Selection.png"));
	pMeshRenderer->SetType(E_RenderAfter);

	m_pSelectionQuad->AddComponent(pMeshRenderer);

	m_pSelectionQuad->GetTransform()->SetPosition(glm::vec3(0, 0.5, 0));
	m_pSelectionQuad->GetTransform()->SetScale(glm::vec3(0, 1.0, 0));
}

UnitController::~UnitController()
{
	
}

void UnitController::OnDestroy()
{
	m_pSelectionQuad->Destroy();
	ClearSelectedShips();

	for (BaseShip* pShip : m_vpVisibleEnemyShips)
	{
		pShip->GetHPBar()->Hide();
		pShip->Destroyed.UnSub(this);
	}
	
	for (BaseShip* pShip : m_vpInvisibleEnemyShips)
		pShip->Destroyed.UnSub(this);
}

void UnitController::Start()
{
	Requires<NetworkIdentity>();
}

void UnitController::Update()
{
	if (!GetIdentity()->HasAuthority()) return;
	UpdateInput();

	if (!m_vpShipsSelected.empty() && !m_vpShipsSelected.front()->IsDead())
	{
		BaseShip* pShip = m_vpShipsSelected.front();

		glm::vec3 v3Position = m_pSelectionQuad->GetTransform()->GetPosition();
		glm::vec3 v3PositionObj = pShip->GetObject()->GetTransform()->GetPosition();

		m_pSelectionQuad->SetEnabled(true);
		m_pSelectionQuad->GetTransform()->SetPosition(
			glm::vec3(v3PositionObj.x, v3Position.y, v3PositionObj.z)
		);

		/* Grow */
		glm::vec3 v3Scale = m_pSelectionQuad->GetTransform()->GetScale();

		if (v3Scale.x < 10)
		{
			v3Scale.x += GameTime::GetDeltaTime() * 30;
			v3Scale.z += GameTime::GetDeltaTime() * 30;

			m_pSelectionQuad->GetTransform()->SetScale(glm::vec3(v3Scale.x, 1.0, v3Scale.z));
		}
	}
	else
	{
		glm::vec3 v3Scale = m_pSelectionQuad->GetTransform()->GetScale();

		if (v3Scale.x > 0)
		{
			v3Scale.x -= GameTime::GetDeltaTime() * 30;
			v3Scale.z -= GameTime::GetDeltaTime() * 30;

			m_pSelectionQuad->GetTransform()->SetScale(glm::vec3(v3Scale.x, 1.0, v3Scale.z));
		}
		else m_pSelectionQuad->SetEnabled(false);
	}
	
	UpdateManagedShipsSight();
}

void UnitController::AddShip(BaseShip* a_pTargetShip)
{
	auto found = std::find(m_vpShips.begin(), m_vpShips.end(), a_pTargetShip);
	if (found != m_vpShips.end()) return;
	
	// Add ship to unit control
	m_vpShips.push_back(a_pTargetShip);
}

void UnitController::RemoveShip(BaseShip* a_pTargetShip)
{
	auto found = std::find(m_vpShips.begin(), m_vpShips.end(), a_pTargetShip);
	
	if (found != m_vpShips.end())
	{
		// Remove ship from unit control
		m_vpShips.erase(found);
	}
}

const std::vector<BaseShip*>& UnitController::GetManagedShips() const 
{
	return m_vpShips;
}

void UnitController::AddEnemyShip(BaseShip* a_pTargetShip)
{
	auto found = std::find(m_vpVisibleEnemyShips.begin(), m_vpVisibleEnemyShips.end(), a_pTargetShip);
        
	if (found == m_vpVisibleEnemyShips.end())
	{
		m_vpVisibleEnemyShips.push_back(a_pTargetShip);
        
		std::function<void(Component*)> lamba = [&](Component* a_pShip) 
		{
			RemoveEnemyShip(static_cast<BaseShip*>(a_pShip));
		};
        
		a_pTargetShip->Destroyed.Sub(lamba, this);
	}
}

void UnitController::AddEnemyShips(const std::vector<BaseShip*>& a_vpTargetShips)
{
	for (auto & i : a_vpTargetShips)
		AddEnemyShip(i);
}

unsigned int UnitController::GetNumAliveShips()
{
	unsigned int count = 0;
	for (BaseShip* pShip : m_vpShips)
	{
		if (!pShip->IsDead())
			++count;
	}
	return count;
}

void UnitController::UpdateInput()
{
	if (GetInput()->MousePressed(Input::E_MOUSE_LEFT))
		HandleLeftClick();
	else if (GetInput()->MousePressed(Input::E_MOUSE_RIGHT) && m_vpShipsSelected.size() > 0)
		HandleRightClick();
}

void UnitController::HandleLeftClick()
{
	GameObject* pObject = nullptr;
	glm::vec3 collisionpoint;
		
	if (GetCamera()->RayCastWorldOnMousePosition(pObject, collisionpoint))
	{
		if (pObject && pObject->GetTag() != "WaterCollider")
		{
			BaseShip* pShip = pObject->GetComponent<BaseShip>();
			if (pShip)
			{
				NetworkIdentity* pIdentity = pObject->GetComponent<NetworkIdentity>();
				if (pIdentity && pIdentity->HasAuthority())
				{
					if (HasSelectedShip(pShip))
						ClearSelectedShips();
					else
					{
						ClearSelectedShips();
						AddSelectedShip(pShip);
					}
				}	
			}
		}
#ifndef ANDROID
		else ClearSelectedShips();
#endif
	}

#ifdef ANDROID
  /* Handle left and right click on tap */
  HandleRightClick();
#endif
}

void UnitController::HandleRightClick()
{
	if (m_vpShipsSelected.empty())
		return;

	GameObject* pObject = nullptr;
	glm::vec3 collisionpoint;
		
	if (GetCamera()->RayCastWorldOnMousePosition(pObject, collisionpoint))
	{
		if (pObject)
		{
			if (pObject->GetTag() == "WaterCollider")
			{
				for (BaseShip* pSelectedShip : m_vpShipsSelected)
					pSelectedShip->RunCommand(BaseShipCommands::E_MOVE, pSelectedShip->GetIdentity()->GetId(), collisionpoint);
			}
			else
			{
				BaseShip* pShip = pObject->GetComponent<BaseShip>();
				if (pShip)
				{
					NetworkIdentity* pIdentity = pObject->GetComponent<NetworkIdentity>();
					if (pIdentity && !pIdentity->HasAuthority())
					{
						for (BaseShip* pSelectedShip : m_vpShipsSelected)
						{
							if (IsVisible(pShip))
							{
								const RakNet::RakString s(pShip->GetObject()->GetTag().c_str());
								pSelectedShip->RunCommand(E_ATTACK, pSelectedShip->GetIdentity()->GetId(), s);
							}
						}
					}
				}
			}
		}	
	}
}

Camera* UnitController::GetCamera() const
{
	return Application::GetInstance()->GetSceneManager()->GetActiveScene()->GetCamera();
}

InputManager* UnitController::GetInput() const
{
	return m_pInput;
}

void UnitController::UpdateManagedShipsSight()
{
	// Vector for storing raycast results
	std::vector<BaseShip*> onSightEnemyShips;
	std::vector<BaseShip*> notOnSightEnemyShips;
    
	UpdateManagedShipsView(m_vpInvisibleEnemyShips, onSightEnemyShips);
	
	for (auto& a : onSightEnemyShips)
	{
		if (!a->GetMeshRenderer()->IsEnabled())
		{
			a->GetMeshRenderer()->SetEnabled(true);
			//a->GetParticleRenderer()->SetEnabled(true);
		}

		if (!a->IsDead())
			a->GetHPBar()->Show();
	}
	
	UpdateManagedShipsView(m_vpVisibleEnemyShips, notOnSightEnemyShips);
	
	for (auto it = m_vpVisibleEnemyShips.begin(); it != m_vpVisibleEnemyShips.end(); )
	{
		auto found = std::find(notOnSightEnemyShips.begin(), notOnSightEnemyShips.end(), *it);
		
		if (found == notOnSightEnemyShips.end() && !(*it)->IsDead())
		{
			(*it)->GetHPBar()->Hide();
			(*it)->GetMeshRenderer()->SetEnabled(false);
			//(*it)->GetParticleRenderer()->SetEnabled(false);
			
			m_vpInvisibleEnemyShips.push_back(*it);
			it = m_vpVisibleEnemyShips.erase(it);
		}
		else
		{
			++it;
		}
	}
	
	for (auto & a : onSightEnemyShips)
	{
		auto found = std::find(m_vpInvisibleEnemyShips.begin(), m_vpInvisibleEnemyShips.end(), a);
		if (found != m_vpInvisibleEnemyShips.end())
			m_vpInvisibleEnemyShips.erase(found);
		
		m_vpVisibleEnemyShips.push_back(a);
	}
}

void UnitController::UpdateManagedShipsView(const std::vector<BaseShip*>& a_vpShipsToCheck, std::vector<BaseShip*>& a_vpShipsVisible)
{
	for (auto & i : m_vpShips)
		i->GetShipsOnSight(a_vpShipsToCheck, a_vpShipsVisible);
}

void UnitController::RemoveEnemyShip(BaseShip* a_pTargetShip)
{
	auto visiblefound = std::find(m_vpVisibleEnemyShips.begin(), m_vpVisibleEnemyShips.end(), a_pTargetShip);
    
	if (visiblefound != m_vpVisibleEnemyShips.end())
		m_vpVisibleEnemyShips.erase(visiblefound);
	
	auto invisiblefound = std::find(m_vpInvisibleEnemyShips.begin(), m_vpInvisibleEnemyShips.end(), a_pTargetShip);
    
	if (invisiblefound != m_vpInvisibleEnemyShips.end())
		m_vpInvisibleEnemyShips.erase(invisiblefound);
}

bool UnitController::IsVisible(BaseShip* a_pTargetShip)
{
	for (BaseShip* pShip : m_vpVisibleEnemyShips)
	{
		if (pShip == a_pTargetShip)
			return true;
	}
	return false;
}

void UnitController::AddSelectedShip(BaseShip* a_pShip)
{
	a_pShip->GetHPBar()->Show();
	a_pShip->SetSelected(true);
	m_vpShipsSelected.push_back(a_pShip);
}

void UnitController::RemoveSelectedShip(BaseShip* a_pShip)
{
	a_pShip->GetHPBar()->Hide();
	a_pShip->SetSelected(false);
	m_vpShipsSelected.erase(
		std::remove(m_vpShipsSelected.begin(),m_vpShipsSelected.end(),a_pShip),
		m_vpShipsSelected.end()
	);
}

void UnitController::ClearSelectedShips()
{
	for (BaseShip* pShip : m_vpShipsSelected)
	{
		pShip->GetHPBar()->Hide();
		pShip->SetSelected(false);
	}

	m_vpShipsSelected.clear();
}

bool UnitController::HasSelectedShip(BaseShip* a_pShip) const
{
	const auto it = std::find(m_vpShipsSelected.begin(), m_vpShipsSelected.end(), a_pShip);
	return it != m_vpShipsSelected.end();
}
