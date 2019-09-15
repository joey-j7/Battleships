#pragma once

#include "Model.h"

class OBJ : public Model
{
public:
	OBJ() = default;
	void Load(const std::string& a_sFilePath) override;

private:
	std::string m_sFolderPath;
};
