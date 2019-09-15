#pragma once
 
#include <unordered_map>
#include "ReferencedObject.h"

#include "Debug.h"
#include "Utils.h"

template <typename T>
class ReferenceManager {
	static_assert(std::is_base_of<ReferencedObject, T>::value, "Type must derive from ReferencedObject");

public:
	ReferenceManager() = default;
	~ReferenceManager() {
		ClearAll();
	}
 
	T* AddReference(std::string& a_sReference) {
		a_sReference = GetPlatformPath(a_sReference);
		std::string sReference = a_sReference.empty() ? " " : a_sReference;
		const auto itIter = m_mReferences.find(sReference);

		if (itIter != m_mReferences.end()) {
			m_mReferences[sReference]->AddReference();
			Debug::Log("Adding reference for object: " + sReference + " " + to_string(itIter->second->GetReferenceCount()));

			return m_mReferences[sReference];
		}

		T* pReference = new T();
		Debug::Log("Loading object to memory: " + sReference);

		m_mReferences.emplace(sReference, pReference);
		return m_mReferences[sReference];
	}

	void RemoveReference(const std::string& a_sReference) {
		std::string sReference = a_sReference.empty() ? " " : a_sReference;
		const auto itIter = m_mReferences.find(sReference);

		if (itIter != m_mReferences.end())
		{
			itIter->second->DelReference();
			Debug::Log("Freeing reference for object: " + sReference + " " + to_string(itIter->second->GetReferenceCount()));

			if (itIter->second->GetReferenceCount() <= 0) {
				Debug::Log("Freeing object from memory: " + sReference);

				delete itIter->second;
				itIter->second = nullptr;
				m_mReferences.erase(itIter);
			}

			return;
		}
	
		Debug::Log("Reference " + sReference + " not found!");
	}
 
	T* GetReference(const std::string& a_sReference) const {
		std::string sReference = a_sReference.empty() ? " " : a_sReference;
		const auto itIter = m_mReferences.find(sReference);
		if (itIter == m_mReferences.end()) return nullptr;
	
		return itIter->second;
	}

	void ClearAll() {
		for (auto itIter = m_mReferences.begin(); itIter != m_mReferences.end(); itIter++)
		{
			if (itIter->second)
				delete itIter->second;
		}

		m_mReferences.clear();
	}
 
private:
	std::unordered_map<std::string, T*> m_mReferences;
};
