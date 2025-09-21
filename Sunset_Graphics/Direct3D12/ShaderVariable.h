#ifndef _DIRECT3D12_SHADERVARIABLE
#define _DIRECT3D12_SHADERVARIABLE

#include "../sunset_graphics_pch.h"
#include "Direct3D_12_def.h"

#include <string>
#include <vector>
#include <d3d12shader.h>

struct ID3D12ShaderReflectionType;
struct ID3D12ShaderReflectionVariable;

namespace DX12
{
	class ShaderVariable
	{
	public:
		class Variable
		{
		public:
			Variable();
			~Variable();

			LPCSTR GetVariableName();
			LPCSTR GetVariableTypeName();
			D3D_SHADER_VARIABLE_CLASS GetVariableType();
			LPCVOID GetDataAddress();
			UINT GetCountOfElement();
			UINT GetCountOfMember();

		private:
			void CreateVariable(
				ID3D12ShaderReflectionType* pType,
				LPCH& pCurrentData
			);

			void CreateStructMember(
				ID3D12ShaderReflectionType* pType,
				LPCH& pCurrentData
			);

			void CreateStruct(
				ID3D12ShaderReflectionType* pType,
				LPCH& pCurrentData
			);

		private:
			LPVOID pData; //ShaderVariableの実体(子がある場合は先頭のポインタ)

			std::string VariableName;
			std::string TypeName;
			D3D_SHADER_VARIABLE_TYPE Type;
			D3D_SHADER_VARIABLE_CLASS Class;
			UINT Element = 0; //配列の数（単体の場合は 0 ）
			UINT Member = 0; //構造体のメンバーの数
			UINT Column = 0; //横の要素数
			UINT Row = 0; //縦の要素数

			std::vector<Variable> members; //この変数が構造体の時のメンバーデータ保管場所

			friend class ShaderVariable;
			friend class Shader;
		};
		typedef Variable* LPVariable;

		ShaderVariable();
		~ShaderVariable();

	protected:
		void Create(
			ID3D12ShaderReflectionVariable* pShaderReflectionVariable,
			LPVOID& pData
		);

	protected:
		ID3D12ShaderReflectionVariable* variable;		
		LPVariable pVariable;

		LPVOID pData;

	private:
		UINT size_of_byte;

		friend class Shader;
	};
}

#endif // !_DIRECT3D12_SHADERVARIABLE



