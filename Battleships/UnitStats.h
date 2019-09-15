#pragma once

#include <algorithm>

class BaseShip;

class UnitStats
{
public:
	UnitStats(BaseShip* a_pShip)
	{
		m_pShip = a_pShip;
	}

	unsigned int GetHP() const { return m_uiHP; }
	unsigned int GetMaxHP() const { return m_uiMaxHP; }

	void AddDamage(unsigned int a_uiDamage);

private:
	BaseShip* m_pShip = nullptr;

	unsigned int m_uiMaxHP = 255;
	unsigned int m_uiHP = GetMaxHP();
};
