#include "ShaderVariable.h"

#include <d3d12.h>
#include <d3d12shader.h>
#include "d3dx12.h"
#include <dxgi1_6.h>

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

namespace DX12
{
	ShaderVariable::ShaderVariable() : pVariable(nullptr)
	{

	}

	ShaderVariable::~ShaderVariable()
	{

	}

	ShaderVariable::Variable::Variable() : pData(nullptr)
	{

	}
	ShaderVariable::Variable::~Variable()
	{
		if (pData != nullptr) delete[](pData);
	}

	LPCSTR ShaderVariable::Variable::GetVariableName()
	{
		return VariableName.c_str();
	}

	LPCSTR ShaderVariable::Variable::GetVariableTypeName()
	{
		return TypeName.c_str();
	}

	D3D_SHADER_VARIABLE_CLASS ShaderVariable::Variable::GetVariableType()
	{
		return Class;
	}

	LPCVOID ShaderVariable::Variable::GetDataAddress()
	{
		return pData;
	}

	UINT ShaderVariable::Variable::GetCountOfElement()
	{
		return Element;
	}

	UINT ShaderVariable::Variable::GetCountOfMember()
	{
		return Member;
	}

	void ShaderVariable::Variable::CreateStructMember(
		ID3D12ShaderReflectionType* pType,
		LPCH& pCurrentData
	)
	{
		D3D12_SHADER_TYPE_DESC desc;
		pType->GetDesc(&desc);
		
		TypeName = desc.Name;
		Class = desc.Class;
		
		Member = desc.Members;
		members.resize(desc.Members);
		for (size_t i = 0; i < desc.Members; i++)
		{
			members[i].VariableName = pType->GetMemberTypeName((UINT)i);
			members[i].CreateVariable(pType->GetMemberTypeByIndex((UINT)i), pCurrentData);
		}
	}

	void ShaderVariable::Variable::CreateStruct(
		ID3D12ShaderReflectionType* pType,
		LPCH& pCurrentData
	)
	{
		members.resize(Element ? Element : 1);
		for (size_t index = 0; index < members.size(); index++)
		{
			members[index].CreateStructMember(pType, pCurrentData);
		}
	}

#define SHADER_VARIABLE_ARRAYSIZE(desc) (desc.Columns * desc.Rows * (desc.Elements ? desc.Elements : 1))

	void ShaderVariable::Variable::CreateVariable(
		ID3D12ShaderReflectionType* pType, 
		LPCH& pCurrentData
	)
	{
		D3D12_SHADER_TYPE_DESC desc;
		pType->GetDesc(&desc);

		TypeName = desc.Name;
		Class = desc.Class;
		Type = desc.Type;
		Element = desc.Elements;
		
		switch (desc.Class)
		{
		case D3D_SVC_STRUCT:
			CreateStruct(pType, pCurrentData);
			break;

		case D3D_SVC_SCALAR:
		case D3D_SVC_VECTOR:
		case D3D_SVC_MATRIX_COLUMNS:
		case D3D_SVC_MATRIX_ROWS:

			Column = desc.Columns;
			Row = desc.Rows;
		
			switch (desc.Type)
			{
			case D3D_SVT_BOOL:
				pData = new(pCurrentData) BOOL[SHADER_VARIABLE_ARRAYSIZE(desc)];
				pCurrentData += sizeof(BOOL) * SHADER_VARIABLE_ARRAYSIZE(desc);
				break;
			case D3D_SVT_INT:
				pData = new(pCurrentData) INT[SHADER_VARIABLE_ARRAYSIZE(desc)];
				pCurrentData += sizeof(INT) * SHADER_VARIABLE_ARRAYSIZE(desc);
				break;
			case D3D_SVT_FLOAT:
				pData = new(pCurrentData) FLOAT[SHADER_VARIABLE_ARRAYSIZE(desc)];
				pCurrentData += sizeof(FLOAT) * SHADER_VARIABLE_ARRAYSIZE(desc);
				break;
			case D3D_SVT_STRING:
				pData = new(pCurrentData) CHAR[SHADER_VARIABLE_ARRAYSIZE(desc)];
				pCurrentData += sizeof(CHAR) * SHADER_VARIABLE_ARRAYSIZE(desc);
				break;
			case D3D_SVT_UINT:
				pData = new(pCurrentData) UINT[SHADER_VARIABLE_ARRAYSIZE(desc)];
				pCurrentData += sizeof(UINT) * SHADER_VARIABLE_ARRAYSIZE(desc);
				break;
			case D3D_SVT_UINT8:
				pData = new(pCurrentData) UINT8[SHADER_VARIABLE_ARRAYSIZE(desc)];
				pCurrentData += sizeof(UINT8) * SHADER_VARIABLE_ARRAYSIZE(desc);
				break;
			case D3D_SVT_DOUBLE:
				pData = new(pCurrentData) DOUBLE[SHADER_VARIABLE_ARRAYSIZE(desc)];
				pCurrentData += sizeof(DOUBLE) * SHADER_VARIABLE_ARRAYSIZE(desc);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}

	void ShaderVariable::Create(
		ID3D12ShaderReflectionVariable* pShaderReflectionVariable,
		LPVOID& pDataStart
	)
	{
		variable = pShaderReflectionVariable;
		
		D3D12_SHADER_VARIABLE_DESC desc;
		variable->GetDesc(&desc);

		size_of_byte = desc.Size;
		pData = pDataStart;
		
		pVariable = new Variable;
		pVariable->VariableName = desc.Name;
		pVariable->CreateVariable(variable->GetType(), (LPCH&)(pDataStart));
	}
}