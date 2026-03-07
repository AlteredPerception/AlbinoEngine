#pragma once
#include <string>

namespace AlbinoEngine
{
	class CBufferVariable
	{
	public:
		CBufferVariable();

		CBufferVariable(std::string varName, unsigned int startOffset, unsigned int size, unsigned int flags);

		~CBufferVariable() {}

		std::string getVariableName() const { return m_pVarName; }

		unsigned int getStartOffset() const { return m_pStartOffset; }

		unsigned int getSize() const { return m_pSize; }

		unsigned int getFlags() const { return m_pFlags; }

	protected:
		std::string m_pVarName;
		unsigned int m_pStartOffset;
		unsigned int m_pSize;
		unsigned int m_pFlags;
	};
}