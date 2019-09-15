#include "UnitStats.h"
#include "BaseShip.h"

void UnitStats::AddDamage(unsigned a_uiDamage)
{
	if (m_pShip->IsDead()) return;

	if (a_uiDamage >= m_uiHP)
	{
		m_uiHP = 0;
		m_pShip->OnDeath();

		return;
	}

	m_uiHP -= a_uiDamage;
}
