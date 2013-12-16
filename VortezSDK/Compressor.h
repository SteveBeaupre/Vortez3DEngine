#ifndef _CCOMPRESSOR_H_
#define _CCOMPRESSOR_H_
#ifdef __cplusplus

/*#ifndef _EXP_FUNC
	#define _EXP_FUNC __declspec(dllexport)
#endif*/

#include <Windows.h>
#include <Stdio.h>
#include "SafeKill.h"	

#include "Preproc.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define COMPRESSOR_ROOT_NODE    0
#define COMPRESSOR_NORMAL_NODE  1
#define COMPRESSOR_LEAF_NODE    2

#ifdef COMPILE_FOR_BORLAND
// Create an abstract class of the object to export
class ACompressor {
public:
	virtual DWORD _INTERNAL_LINKAGE EncodeToFile(BYTE* pBuffer, const DWORD BufferSize, FILE* OutF) = 0;
	virtual DWORD _INTERNAL_LINKAGE DecodeFromFile(BYTE* pBuffer, const DWORD ExpectedBufferSize, FILE* InF) = 0;
	virtual DWORD _INTERNAL_LINKAGE DecodeFromMemory(BYTE* pDstBuffer, BYTE *pSrcBuffer, DWORD dwUncompressedSize, DWORD dwCompressedSize, DWORD NumEncodedChar, BYTE *pBinCodes) = 0;


	virtual void _INTERNAL_LINKAGE Free() = 0;
};
#endif

#ifdef COMPILE_FOR_BORLAND
class CCompressor  : public ACompressor {
#else
class _EXP_FUNC CCompressor {
#endif
public:

// Our Stats struct
#ifdef COMPILE_FOR_BORLAND
struct StatsStruct {
#else
struct _EXP_FUNC StatsStruct {
#endif
	BYTE Char;
	UINT Frequency;

	struct StatsStruct *ChildNode;
	struct StatsStruct *ParentNode;
};

#ifdef COMPILE_FOR_BORLAND
class CStats {
#else
class _EXP_FUNC CStats {
#endif
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

#ifdef COMPILE_FOR_BORLAND
struct BinCodeStruct {
#else
struct _EXP_FUNC BinCodeStruct {
#endif
	UINT Frequency;
	WORD BinCode;
	BYTE BitsCount;
};

//////////////////////////

#ifdef COMPILE_FOR_BORLAND
struct TreeStruct {
#else
struct _EXP_FUNC TreeStruct {
#endif
	UINT Frequency;
	BYTE Char;
	BYTE NodeType;
	WORD BinaryCode;

	struct TreeStruct *LeftChild;
	struct TreeStruct *RightChild;
	struct TreeStruct *Parent;
};

#ifdef COMPILE_FOR_BORLAND
struct TreeBaseNodeListStruct {
#else
struct _EXP_FUNC TreeBaseNodeListStruct {
#endif
	TreeStruct NodeDat;

	struct TreeBaseNodeListStruct *Parent;
	struct TreeBaseNodeListStruct *Child;
};

/////////////////////////////////////////////

#ifdef COMPILE_FOR_BORLAND
struct PriorityQueueStruct {
#else
struct _EXP_FUNC PriorityQueueStruct {
#endif
	UINT Priority;
	BYTE Char;

	struct TreeStruct *pTreeNode;

	struct PriorityQueueStruct *ParentNode;
	struct PriorityQueueStruct *ChildNode;
};

#ifdef COMPILE_FOR_BORLAND
class CPriorityQueue {
#else
class _EXP_FUNC CPriorityQueue {
#endif
public:
	CPriorityQueue();
	~CPriorityQueue();
private:
	TreeBaseNodeListStruct *pTreeLeafsNode;
	TreeBaseNodeListStruct TreeLeafsNode;

	PriorityQueueStruct  RootNode;
	PriorityQueueStruct* GetLastNode();
	TreeStruct* GetBaseTreeNode(BYTE Indx);

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

#ifdef COMPILE_FOR_BORLAND
class CBitStream {
#else
class _EXP_FUNC CBitStream {
#endif
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

#ifdef COMPILE_FOR_BORLAND
class CHuffmanCoder {
#else
class _EXP_FUNC CHuffmanCoder {
#endif
private:
	CStats Stats;
	CPriorityQueue PriorityQueue;
public:
	DWORD EncodeToFile(BYTE* pBuffer, const DWORD BufferSize, FILE* OutF);
	DWORD DecodeFromFile(BYTE* pBuffer, const DWORD ExpectedBufferSize, FILE* InF);
	DWORD DecodeFromMemory(BYTE* pDstBuffer, BYTE *pSrcBuffer, DWORD dwUncompressedSize, DWORD dwCompressedSize, DWORD NumEncodedChar, BYTE *pBinCodes);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	DWORD _INTERNAL_LINKAGE EncodeToFile(BYTE* pBuffer, const DWORD BufferSize, FILE* OutF);
	DWORD _INTERNAL_LINKAGE DecodeFromFile(BYTE* pBuffer, const DWORD ExpectedBufferSize, FILE* InF);
	DWORD _INTERNAL_LINKAGE DecodeFromMemory(BYTE* pDstBuffer, BYTE *pSrcBuffer, DWORD dwUncompressedSize, DWORD dwCompressedSize, DWORD NumEncodedChar, BYTE *pBinCodes);

#ifdef COMPILE_FOR_BORLAND
	void _INTERNAL_LINKAGE Free(){if(this) delete this;}
#endif
};

#endif
#endif //_CCOMPRESSOR_H_