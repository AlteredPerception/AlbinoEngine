#include "CBufferVariable.h"

namespace AlbinoEngine
{
	CBufferVariable::CBufferVariable()
	{
		this->m_pVarName = "";
		this->m_pStartOffset = 0;
		this->m_pSize = 0;
		this->m_pFlags = 0;
	}

	CBufferVariable::CBufferVariable(std::string varName, unsigned int startOffset, unsigned int size, unsigned int flags)
	{
		this->m_pVarName = varName;
		this->m_pStartOffset = startOffset;
		this->m_pSize = size;
		this->m_pFlags = flags;
	}
	
}