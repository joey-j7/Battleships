#pragma once
#include "NetworkBehavior.h"

#include <vector>

class Inputmanager;
class Camera;
class BaseShip;

class ShaderProgram;
class Shape;
class Texture;

class UnitController : public NetworkBehavior
{
public:
	UnitController(GameObject* a_pObj);
	~UnitController();

	RakNet::RakString GetName() const override { return "UnitController"; }
	
	void OnDestroy() override;
	void Start() override;
	void Update() override;
	
	void AddShip(BaseShip* a_pTargetShip);
	void RemoveShip(BaseShip* a_pTargetShip);
	const std::vector<BaseShip*>& GetManagedShips() const;
	
	void AddEnemyShip(BaseShip* a_pTargetShip);
	void AddEnemyShips(const std::vector<BaseShip*>& a_vpTargetShips);

	bool IsVisible(BaseShip* a_pTargetShip);

	unsigned int GetNumAliveShips();

private:
	void UpdateInput();
	void HandleLeftClick();
	void HandleRightClick();
	
	void UpdateManagedShipsSight();
	void UpdateManagedShipsView(const std::vector<BaseShip*>& a_vpShipsToCheck, std::vector<BaseShip*>& a_vpShipsVisible);
	
	void RemoveEnemyShip(BaseShip* a_pTargetShip);
	void AddSelectedShip(BaseShip* a_pShip);
	void RemoveSelectedShip(BaseShip* a_pShip);
	void ClearSelectedShips();
	bool HasSelectedShip(BaseShip* a_pShip) const;

	Camera* GetCamera() const;
	InputManager* GetInput() const;

	std::vector<BaseShip*> m_vpShips;
	std::vector<BaseShip*> m_vpShipsSelected;
	
	std::vector<BaseShip*> m_vpVisibleEnemyShips;
	std::vector<BaseShip*> m_vpInvisibleEnemyShips;

	GameObject* m_pSelectionQuad;
};
