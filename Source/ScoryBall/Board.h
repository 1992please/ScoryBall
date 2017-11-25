#pragma  once
#include "CoreMinimal.h"


enum class ECellType : uint8
{
	Nothing = 0,
	Block = 1,
	Turret = 2,
	Battery = 3
};

struct FBoardCoords
{
	int32 X;
	int32 Y;
	FBoardCoords(int32 x, int32 y) : X(x), Y(y) {}
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

	void GetEmptyCellCoords(TArray<FBoardCoords>& BoardCoords)
	{
		BoardCoords.Empty();
		for (int i = 0; i < m_RowSize; i++)
		{
			for (int j = 0; j < m_ColSize; j++)
			{
				if (m_Rows[i][j].level1 == ECellType::Nothing)
					BoardCoords.Add(FBoardCoords(i, j));
			}
		}
	}

	int32 GetNoOfEmptyCells()
	{
		int32 NEmptyCells = 0;
		for (int i = 0; i < m_RowSize; i++)
		{
			for (int j = 0; j < m_ColSize; j++)
			{
				if (m_Rows[i][j].level1 == ECellType::Nothing)
					NEmptyCells++;
			}
		}
		return NEmptyCells;
	}
};