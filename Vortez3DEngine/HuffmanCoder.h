#ifndef C_HUFFMAN_CODER_DLL_H
#define C_HUFFMAN_CODER_DLL_H
#ifdef __cplusplus

#include <Windows.h>
#include <Stdio.h>
#include "SafeKill.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ROOT_NODE    0
#define NORMAL_NODE  1
#define LEAF_NODE    2

// Our Stats struct
struct StatsStruct {
	BYTE Char;
	UINT Frequency;

	struct StatsStruct *ChildNode;
	struct StatsStruct *ParentNode;
};

class CStats {
public:
	CStats();
	~CStats();
private:
	StatsStruct RootNode;

	void         InitNode(StatsStruct *pNode, StatsStruct *pParentNode, BYTE ch);
	StatsStruct* GetLastNode();
	StatsStruct* AddChildNode(StatsStruct *pNode, BYTE ch);
	bool         RemoveChildNode(StatsStruct *pNode);
	void         ExchangeNodes(StatsStruct *pNode1, StatsStruct *pNode2);
	void         RemoveEmptyStats();
public:
	void Initialize();
    void IncFrequency(BYTE Indx, DWORD Value = 1);
	void KillNodes();

    UINT GetNodeCount();
    void GetValue(BYTE Indx, BYTE *pChar, UINT *pFrequency);

	UINT SortStats();
	
	void WriteStats(char *fname);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct BinCodeStruct {
	UINT Frequency;
	WORD BinCode;
	BYTE BitsCount;
};

//////////////////////////

struct TreeStruct {
	UINT Frequency;
	BYTE Char;
	BYTE NodeType;
	WORD BinaryCode;

	struct TreeStruct *LeftChild;
	struct TreeStruct *RightChild;
	struct TreeStruct *Parent;
};

struct TreeBaseNodeListStruct {
	TreeStruct NodeDat;

	struct TreeBaseNodeListStruct *Parent;
	struct TreeBaseNodeListStruct *Child;
};

/////////////////////////////////////////////

struct PriorityQueueStruct {
	UINT Priority;
	BYTE Char;

	struct TreeStruct *pTreeNode;

	struct PriorityQueueStruct *ParentNode;
	struct PriorityQueueStruct *ChildNode;
};

class CPriorityQueue {
public:
	CPriorityQueue();
	~CPriorityQueue();
private:
	TreeBaseNodeListStruct TreeLeafsNode;

	PriorityQueueStruct  RootNode;
	PriorityQueueStruct* GetLastNode();

	void KillTreeNode(TreeStruct *pNode);
	void CombineTwoLowestFreqNodes();
	bool RemoveFirstNode();
	PriorityQueueStruct* InsertNode(UINT Priority, BYTE ch);
public:
    UINT GetNodeCount();

	void AddLastNode(UINT Priority, BYTE ch);
	void ClearNodes();

	void BuildTree();
	void KillTree();

	TreeStruct* GetTreeRootNode();

	DWORD GenBinCodes(BinCodeStruct *pBinCodes);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBitStream {
public:
	CBitStream();
	~CBitStream();
private:
	BYTE *pBuffer;
	bool IsAllocated;
	bool AllocateExtraDWORD;

	DWORD BufSize;
	DWORD BufIndx;
	BYTE  BitIndx;

	bool TestBit (WORD  bitField, char bitNum){return (bitField & (1<<bitNum)) > 0;}
	void SetBit  (BYTE* bitField, char bitNum){*bitField |= (1 << bitNum);}
	void ClearBit(BYTE* bitField, char bitNum){*bitField &= ~(1 << bitNum);}

	void UpdateCounters(BYTE BitsProcessed);
public:
	void Allocate(DWORD Size);
	void Fill(BYTE* pBuf);
	void Erase();
	void Free();

	void Save(FILE *OutF);

	void EncodeNextChar(WORD wBinCode, BYTE bBitsCount);
	BYTE DecodeNextChar(TreeStruct *pRootNode);
};

TreeStruct* GetBaseTreeNode(BYTE Indx);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CHuffmanCoder {
private:
	CStats Stats;
	CPriorityQueue PriorityQueue;
public:
	#ifndef WIN64
	DWORD EncodeToFile(BYTE* pBuffer, const DWORD BufferSize, FILE* OutF);
	DWORD DecodeFromFile(BYTE* pBuffer, const DWORD ExpectedBufferSize, FILE* InF);
	#else
	size_t EncodeToFile(BYTE* pBuffer, const size_t BufferSize, FILE* OutF);
	size_t DecodeFromFile(BYTE* pBuffer, const size_t ExpectedBufferSize, FILE* InF);
	#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef WIN64
DWORD Encode(BYTE* pBuffer, const DWORD BufferSize, FILE* OutF);
DWORD Decode(BYTE* pBuffer, const DWORD ExpectedBufferSize, FILE* InF);
#else
size_t Encode(BYTE* pBuffer, const size_t BufferSize, FILE* OutF);
size_t Decode(BYTE* pBuffer, const size_t ExpectedBufferSize, FILE* InF);
#endif


#endif
#endif //C_HUFFMAN_CODER_DLL_H