#pragma once
 
class ReferencedObject {
public:
	void AddReference() {
		m_uiRefCount++;
	}
 
	void DelReference() {
		if (m_uiRefCount > 0) {
			m_uiRefCount--;
		}
	}
 
	const unsigned int& GetReferenceCount() const {
		return m_uiRefCount;
	}

	bool IsLoaded() const
	{
		return m_bIsLoaded;
	}
 
protected:
	bool m_bIsLoaded = false;
	unsigned int m_uiRefCount = 1;
};