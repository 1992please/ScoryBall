#pragma  once
#include "CoreMinimal.h"


enum class ECellType : uint8
{
	Nothing = 0,
	Block = 1,
	Turret= 2
};

struct FCell
{
	ECellType level1;
	ECellType level2;
};

struct FRow
{
	TArray<FCell> m_Cells;
	FRow() {}
	FORCEINLINE void Resize(const int32 Size)
	{
		m_Cells.SetNum(Size);
	}

	FORCEINLINE FCell& operator[](const int32 Index)
	{
		return m_Cells[Index];
	}
};

struct FBoard
{
	TArray<FRow> m_Rows;
	int32 m_RowSize;
	int32 m_ColSize;

	FBoard() : m_RowSize(0), m_ColSize(0) {}

	FORCEINLINE void Resize(const int32 nRows, const int32 nCols)
	{
		m_RowSize = nRows;
		m_ColSize = nCols;
		m_Rows.SetNum(nRows);
		for (int i = 0; i < nRows; i++)
		{
			m_Rows[i].Resize(nCols);
		}
	}

	FORCEINLINE FRow& operator[](const int32 Index)
	{
		return m_Rows[Index];
	}
};