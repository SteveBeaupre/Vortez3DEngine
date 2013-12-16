#include "HuffmanCoder.h"

TreeBaseNodeListStruct *pTreeLeafsNode = NULL;


#ifndef WIN64
DWORD Encode(BYTE* pBuffer, const DWORD BufferSize, FILE* OutF)
#else
size_t Encode(BYTE* pBuffer, const size_t BufferSize, FILE* OutF)
#endif
{
	CHuffmanCoder HuffmanCoder;
	return HuffmanCoder.EncodeToFile(pBuffer, BufferSize, OutF);
}

#ifndef WIN64
DWORD Decode(BYTE* pBuffer, const DWORD ExpectedBufferSize, FILE* InF)
#else
size_t Decode(BYTE* pBuffer, const size_t ExpectedBufferSize, FILE* InF)
#endif
{
	CHuffmanCoder HuffmanCoder;
	return HuffmanCoder.DecodeFromFile(pBuffer, ExpectedBufferSize, InF);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef WIN64
DWORD CHuffmanCoder::EncodeToFile(BYTE* pBuffer, const DWORD BufferSize, FILE* OutF)
#else
size_t CHuffmanCoder::EncodeToFile(BYTE* pBuffer, const size_t BufferSize, FILE* OutF)
#endif
{
	#ifndef WIN64
	DWORD NumBytesWritten = 0;
	#else
	size_t NumBytesWritten = 0;
	#endif

	// Errors handling
	if(pBuffer == NULL || BufferSize == 0){return 0;}

	////////////////////////////////////////////////////////////////////////////////////
	// Create a char. distribution table sorted by frequency
	////////////////////////////////////////////////////////////////////////////////////

	// Initialize the list
	Stats.Initialize();
	// Build our char. distribution statistics
	for(DWORD Cpt = 0; Cpt < BufferSize; Cpt++)
		Stats.IncFrequency(pBuffer[Cpt]);
	// Sort them by frequency and remove the useless one
	DWORD NumNodesCount = Stats.SortStats();

	////////////////////////////////////////////////////////////////////////////////////
	// Build our huffman tree
	////////////////////////////////////////////////////////////////////////////////////

	// Convert our stats in a Priority list used to build the tree...
	DWORD Cpt = 0;
	for(Cpt = 0; Cpt < NumNodesCount; Cpt++){
		BYTE b = 0;
		UINT Freq = 0;
		Stats.GetValue((BYTE)Cpt, &b, &Freq);
		PriorityQueue.AddLastNode(Freq, b);
	}
	// Delete the stats nodes list
	Stats.KillNodes();
	// Build the tree...
	PriorityQueue.BuildTree();

	////////////////////////////////////////////////////////////////////////////////////
	// Fill a BinCode structure, used to encode our characters
	////////////////////////////////////////////////////////////////////////////////////

	// Create and initialise our BinCode struct
	BinCodeStruct BinCodes[256];
	ZeroMemory(&BinCodes[0], sizeof(BinCodeStruct) * 256);
	// This gave us the new binary code for each char.
	DWORD NumEncodedChar = PriorityQueue.GenBinCodes(&BinCodes[0]);

	////////////////////////////////////////////////////////////////////////////////////
	// Write our header
	////////////////////////////////////////////////////////////////////////////////////

	// Write the original data size
	NumBytesWritten += fwrite(&BufferSize, 1, sizeof(DWORD), OutF);		

	// Calculate the size of our encoded data and save it
	DWORD EncodedBufferSize = 0;
	for(Cpt = 0; Cpt < BufferSize; Cpt++)
		EncodedBufferSize += BinCodes[pBuffer[Cpt]].BitsCount;
	bool AddMissingByte = EncodedBufferSize % 8 > 0;
	EncodedBufferSize >>= 3;
	if(AddMissingByte){EncodedBufferSize++;}
	NumBytesWritten += fwrite(&EncodedBufferSize, 1, sizeof(DWORD), OutF);		

	////////////////////////////////////////////////////////////////////////////////////
	// Write the Compression Table
	////////////////////////////////////////////////////////////////////////////////////

	// Write the number of char encoded
	NumBytesWritten += fwrite(&NumEncodedChar, 1, sizeof(DWORD), OutF);
	
	for(Cpt = 0; Cpt < 256; Cpt++){
		if(BinCodes[Cpt].BitsCount > 0){
			NumBytesWritten += fwrite((BYTE*)&Cpt,              1, sizeof(BYTE),  OutF);	
			NumBytesWritten += fwrite(&BinCodes[Cpt].Frequency, 1, sizeof(DWORD), OutF);
			NumBytesWritten += fwrite(&BinCodes[Cpt].BinCode,   1, sizeof(WORD),  OutF);	
			NumBytesWritten += fwrite(&BinCodes[Cpt].BitsCount, 1, sizeof(BYTE),  OutF);	
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Write our encoded data
	////////////////////////////////////////////////////////////////////////////////////

	CBitStream BitStream;
	BitStream.Allocate(EncodedBufferSize);

	for(Cpt = 0; Cpt < BufferSize; Cpt++){
		BYTE ch = pBuffer[Cpt];
		BitStream.EncodeNextChar(BinCodes[ch].BinCode, BinCodes[ch].BitsCount);
	}
	NumBytesWritten += EncodedBufferSize;

	BitStream.Save(OutF);
	BitStream.Free();

	////////////////////////////////////////////////////////////////////////////////////

	PriorityQueue.KillTree();

	return NumBytesWritten;
}


#ifndef WIN64
DWORD CHuffmanCoder::DecodeFromFile(BYTE* pBuffer, const DWORD ExpectedBufferSize, FILE* InF)
#else
size_t CHuffmanCoder::DecodeFromFile(BYTE* pBuffer, const size_t ExpectedBufferSize, FILE* InF)
#endif
{
	#ifndef WIN64
	DWORD NumBytesReaden = 0;
	#else
	size_t NumBytesReaden = 0;
	#endif

	DWORD BufferSize = 0;
	DWORD EncodedBufferSize = 0;
	DWORD NumEncodedChar = 0;

	// Read the original data size
	NumBytesReaden += fread(&BufferSize, 1, sizeof(DWORD), InF);
	// Return false if the size don't match
	if(BufferSize != ExpectedBufferSize){return 0;}

	// Read the encoded data size
	NumBytesReaden += fread(&EncodedBufferSize, 1, sizeof(DWORD), InF);		

	////////////////////////////////////////////////////////////////////////////////////
	// Read the header
	////////////////////////////////////////////////////////////////////////////////////

	// Read the number of char to encode
	NumBytesReaden += fread(&NumEncodedChar, 1, sizeof(DWORD), InF);		

	// Create and initialise our BinCode struct
	BinCodeStruct BinCodes[256];
	ZeroMemory(&BinCodes[0], sizeof(BinCodeStruct) * 256);

	////////////////////////////////////////////////////////////////////////////////////
	// Read back our bincode structure and build the stats table
	////////////////////////////////////////////////////////////////////////////////////

	// Initialize the list
	Stats.Initialize();

	BYTE ch = 0;
	WORD BinCode = 0;
	BYTE BitsCount = 0;
	UINT Freq = 0;
	for(DWORD Cpt = 0; Cpt < NumEncodedChar; Cpt++){
		NumBytesReaden += fread(&ch,        1, sizeof(BYTE),  InF);	
		NumBytesReaden += fread(&Freq,      1, sizeof(DWORD), InF);	
		NumBytesReaden += fread(&BinCode,   1, sizeof(WORD),  InF);	
		NumBytesReaden += fread(&BitsCount, 1, sizeof(BYTE),  InF);	
		
		BinCodes[ch].BinCode   = BinCode;
		BinCodes[ch].BitsCount = BitsCount;
		BinCodes[ch].Frequency = Freq;
		
		Stats.IncFrequency(ch, Freq);
	}

	// Sort them by frequency and remove the useless one
	Stats.SortStats();

	////////////////////////////////////////////////////////////////////////////////////
	// Build our huffman tree
	////////////////////////////////////////////////////////////////////////////////////

	// Convert our stats in a Priority list used to build the tree...
	DWORD Cpt = 0;
	for(Cpt = 0; Cpt < Stats.GetNodeCount(); Cpt++){
		BYTE b = 0;
		UINT Freq = 0;
		Stats.GetValue((BYTE)Cpt, &b, &Freq);

		PriorityQueue.AddLastNode(Freq, b);
	}
	// Build the tree...
	PriorityQueue.BuildTree();
	
	// Delete the stats nodes list
	Stats.KillNodes();

	////////////////////////////////////////////////////////////////////////////////////
	// Read the encoded data
	////////////////////////////////////////////////////////////////////////////////////

	// Create a buffer to read our data into
	BYTE* pEncodedDataBuf = new BYTE[EncodedBufferSize+1];
	ZeroMemory(pEncodedDataBuf, EncodedBufferSize+1);

	// Read the remaning data
	#ifndef WIN64
	DWORD NumBufferBytesRead = fread(pEncodedDataBuf, 1, EncodedBufferSize, InF);
	#else
	size_t NumBufferBytesRead = fread(pEncodedDataBuf, 1, EncodedBufferSize, InF);
	#endif
	NumBytesReaden += NumBufferBytesRead;	
	// Return false if we didn't read enough, should not happen...
	if(NumBufferBytesRead != EncodedBufferSize){
		PriorityQueue.KillTree();
		return 0;
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Decode the data
	////////////////////////////////////////////////////////////////////////////////////


	CBitStream BitStream;
	BitStream.Allocate(EncodedBufferSize);
	BitStream.Fill(pEncodedDataBuf);
	
	for(Cpt = 0; Cpt < BufferSize; Cpt++)
		pBuffer[Cpt] = BitStream.DecodeNextChar(PriorityQueue.GetTreeRootNode());

	BitStream.Free();

	// Free the encoded buffer
	SAFE_DELETE_ARRAY(pEncodedDataBuf);

	////////////////////////////////////////////////////////////////////////////////////

	PriorityQueue.KillTree();

	return NumBytesReaden;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CStats::CStats()
{
	InitNode(&RootNode, NULL, 0);
}

CStats::~CStats()
{
	KillNodes();
}

void CStats::InitNode(StatsStruct *pNode, StatsStruct *pParentNode, BYTE ch)
{
	pNode->Char = ch;
	pNode->Frequency = 0;
	pNode->ChildNode = NULL;
	pNode->ParentNode = pParentNode;
}

void CStats::Initialize()
{
	KillNodes();
	InitNode(&RootNode, NULL, 0);

	StatsStruct *pNode = &RootNode;
	for(int Cpt = 0; Cpt < 256; Cpt++)
		pNode = AddChildNode(pNode, Cpt);
}

void CStats::KillNodes()
{
	StatsStruct *pNode;
	do {
		pNode = GetLastNode();
		if(pNode != &RootNode)
			RemoveChildNode(pNode);
	
	} while(pNode != &RootNode);
}

StatsStruct* CStats::GetLastNode()
{
	StatsStruct *pNode = &RootNode;
	
	while(pNode->ChildNode != NULL)
		pNode = pNode->ChildNode;

	return pNode;
}

StatsStruct* CStats::AddChildNode(StatsStruct *pNode, BYTE ch)
{
	pNode->ChildNode = new StatsStruct;
	InitNode(pNode->ChildNode, pNode, ch);

	return pNode->ChildNode;
}

bool CStats::RemoveChildNode(StatsStruct *pNode)
{
	if(pNode == &RootNode){return false;}

	StatsStruct *child = pNode->ChildNode, *parent = pNode->ParentNode;
	if(parent != NULL){
		
		// Update the parent node
		parent->ChildNode = child;

		// Update the child node
		if(child != NULL)
			child->ParentNode = parent;

		// Delete the node
		SAFE_DELETE_OBJECT(pNode);

		return true;
	}

	return false;
}

void CStats::ExchangeNodes(StatsStruct *pNode1, StatsStruct *pNode2)
{
	if(pNode1 == NULL || pNode1 == &RootNode){return;}
	if(pNode2 == NULL || pNode2 == &RootNode){return;}

	StatsStruct Tmp;
	memcpy(&Tmp.Char,          &pNode1->Char,      sizeof(BYTE));
	memcpy(&pNode1->Char,      &pNode2->Char,      sizeof(BYTE));
	memcpy(&pNode2->Char,      &Tmp.Char,          sizeof(BYTE));
	memcpy(&Tmp.Frequency,     &pNode1->Frequency, sizeof(UINT));
	memcpy(&pNode1->Frequency, &pNode2->Frequency, sizeof(UINT));
	memcpy(&pNode2->Frequency, &Tmp.Frequency,     sizeof(UINT));
}


UINT CStats::SortStats()
{
	// Sort our stats starting with the lowest frequency's
	StatsStruct *i, *j;

	i = RootNode.ChildNode;
	j = i->ChildNode;
	while(i->ChildNode != NULL){
		while(j != NULL){
			
			if(i->Frequency > j->Frequency)
				ExchangeNodes(i, j);

			j = j->ChildNode;
		}
		i = i->ChildNode;
		j = i->ChildNode;
	}	

	StatsStruct* pLastNode = GetLastNode();

	RemoveEmptyStats();
	return GetNodeCount();
}


void CStats::RemoveEmptyStats()
{
	if(RootNode.ChildNode == NULL){return;}
	StatsStruct *pNode = &RootNode;

	while(pNode->ChildNode != NULL){
		pNode = &RootNode;
		if(pNode->ChildNode->Frequency == 0)
			RemoveChildNode(pNode->ChildNode);
		else 
			break;
	}
}

void CStats::IncFrequency(BYTE Indx, DWORD Value)
{
	StatsStruct *pNode = RootNode.ChildNode;
	if(pNode == NULL){return;}
	
	for(int Cpt = 0; Cpt < Indx; Cpt++){
		pNode = pNode->ChildNode;
		if(!pNode){return;}
	}

	pNode->Frequency += Value;
}

void CStats::GetValue(BYTE Indx, BYTE *pChar, UINT *pFrequency)
{
	StatsStruct *pNode = RootNode.ChildNode;
	if(pNode == NULL){return;}
	
	for(int Cpt = 0; Cpt < Indx; Cpt++){
		pNode = pNode->ChildNode;
		if(!pNode){return;}
	}

	*pChar = pNode->Char;
	*pFrequency = pNode->Frequency;
}

UINT CStats::GetNodeCount()
{
	UINT Count = 0;
	StatsStruct *pNode = &RootNode;

	while(pNode->ChildNode != NULL){
		pNode = pNode->ChildNode;
		Count++;
	}

	return Count;
}

void CStats::WriteStats(char *fname)
{
	FILE *Outf = fopen(fname, "wt+");
	if(!Outf){return;}

	StatsStruct *pNode = RootNode.ChildNode;

	char LineBuf[64];
	while(pNode != NULL){
		sprintf(LineBuf, "%2.2X - %8.8X\n", pNode->Char, pNode->Frequency);
		fputs(LineBuf, Outf);
		pNode = pNode->ChildNode;
	}

	fclose(Outf);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Initialize the first node
CPriorityQueue::CPriorityQueue()
{
	pTreeLeafsNode = &TreeLeafsNode;

	RootNode.Priority   = 0;
	RootNode.Char       = 0;
	RootNode.ChildNode  = NULL;
	RootNode.ParentNode = NULL;
}

// Remove all the remaining nodes
CPriorityQueue::~CPriorityQueue()
{
	ClearNodes();
}

// Count the number of nodes
UINT CPriorityQueue::GetNodeCount()
{
	UINT Count = 0;
	PriorityQueueStruct *pNode = &RootNode;

	while(pNode->ChildNode != NULL){
		pNode = pNode->ChildNode;
		Count++;
	}

	return Count;
}



// Return a pointer to the last node
PriorityQueueStruct* CPriorityQueue::GetLastNode()
{
	PriorityQueueStruct *pNode = &RootNode;
	
	while(pNode->ChildNode != NULL)
		pNode = pNode->ChildNode;

	return pNode;
}

// Add a node at the end of the list
void CPriorityQueue::AddLastNode(UINT Priority, BYTE ch)
{
	PriorityQueueStruct *pNode = GetLastNode();

	pNode->ChildNode = new PriorityQueueStruct;

	pNode->ChildNode->Char = ch;
	pNode->ChildNode->Priority  = Priority;
	
	pNode->ChildNode->ChildNode  = NULL;
	pNode->ChildNode->ParentNode = pNode;
}

// Insert a node at a given place
PriorityQueueStruct* CPriorityQueue::InsertNode(UINT Priority, BYTE ch)
{
	PriorityQueueStruct *pNewNode = new PriorityQueueStruct;
	pNewNode->Char = ch;
	pNewNode->Priority = Priority;
	
	PriorityQueueStruct *pNode = GetLastNode();

	// Get a pointer that point where to insert the node
	while(pNode->Priority > pNewNode->Priority)
		pNode = pNode->ParentNode;

	// Update pointers
	pNewNode->ParentNode = pNode;
	pNewNode->ChildNode = pNode->ChildNode;
	if(pNewNode->ChildNode != NULL)
		pNode->ChildNode->ParentNode = pNewNode;
	pNode->ChildNode = pNewNode;

	return pNewNode;
}

// Delete the first node
bool CPriorityQueue::RemoveFirstNode()
{
	PriorityQueueStruct *pNode = RootNode.ChildNode;
	if(pNode == NULL){return false;}

	PriorityQueueStruct *child = pNode->ChildNode, *parent = pNode->ParentNode;

	// Update the parent node
	parent->ChildNode = child;

	// Update the child node
	if(child != NULL)
		child->ParentNode = parent;

	// Delete the node
	SAFE_DELETE_OBJECT(pNode);

	return true;
}

// Delete all the nodes
void CPriorityQueue::ClearNodes()
{
	while(RemoveFirstNode());
}

//////////////////////////////////////////////////////////////////////////////////////////////

// Build our huffman tree... finally...
void CPriorityQueue::CombineTwoLowestFreqNodes()
{
	//Get 2 pointers to our 2 first nodes
	if(GetNodeCount() < 2){return;}
	PriorityQueueStruct *FirstNode = RootNode.ChildNode;
	PriorityQueueStruct *LastNode  = RootNode.ChildNode->ChildNode;

	// Add the two frequency
	UINT NewFreq = FirstNode->Priority + LastNode->Priority;

	// Create a new Tree node
	TreeStruct *pTreeNode = new TreeStruct;

	// Init the new node data
	pTreeNode->Frequency  = NewFreq;
	pTreeNode->Char       = 0;
	pTreeNode->NodeType   = GetNodeCount() > 2 ? NORMAL_NODE : ROOT_NODE;

	// Init the new node pointers
	pTreeNode->Parent     = NULL;
	pTreeNode->LeftChild  = FirstNode->pTreeNode;
	pTreeNode->RightChild = LastNode->pTreeNode;

	// Setup the parent node pointer to our tree pointer via a fucking pointer.........
	FirstNode->pTreeNode->Parent = pTreeNode;
	LastNode->pTreeNode->Parent  = pTreeNode;
	
	// Set the binary code flag
	FirstNode->pTreeNode->BinaryCode = 0x00;
	LastNode->pTreeNode->BinaryCode = 0x01;
	if(GetNodeCount() <= 2)
		pTreeNode->BinaryCode = 0x00;

	// Remove the first two nodes from the list
	RemoveFirstNode();
	RemoveFirstNode();
	
	// Create a new node with the two added frequency
	// Insert this new nodes in the list, it will be sorted automatically...
	PriorityQueueStruct *pNewNode;
	pNewNode = InsertNode(NewFreq, 0);

	// Set our TreeNode pointers...
	pNewNode->pTreeNode = pTreeNode;

	// Remove the last created node if this is the last pass
	if(GetNodeCount() == 1)
		RemoveFirstNode(); // Maybe we should make this node the root node
}

// Build our huffman tree... finally...
void CPriorityQueue::BuildTree()
{
	// How many leafs have we?
	UINT NodeCount = GetNodeCount();

	// Get a pointer to the Root of the leaf nodes list
	TreeBaseNodeListStruct *pTreeBase = &TreeLeafsNode;

	// This will build the base of our tree, or the leafs...
	PriorityQueueStruct *pNode = &RootNode;
	while(pNode->ChildNode != NULL){
		pNode = pNode->ChildNode;

		// Create a new leaf node
		pTreeBase->Child = new TreeBaseNodeListStruct;
		// Update the leaf node pointer to point to this new node
		pTreeBase = pTreeBase->Child;
		
		// Initialize the new node
		pTreeBase->NodeDat.Frequency  = pNode->Priority;
		pTreeBase->NodeDat.Char       = pNode->Char;
		pTreeBase->NodeDat.NodeType   = LEAF_NODE;

		pTreeBase->NodeDat.Parent     = NULL;
		pTreeBase->NodeDat.LeftChild  = NULL;
		pTreeBase->NodeDat.RightChild = NULL;
		
		pTreeBase->Child  = NULL;
		pTreeBase->Parent = pTreeBase;

		// Update the tree node pointer of our leaf nodes list
		pNode->pTreeNode = &pTreeBase->NodeDat;
	}
	
	// Build the tree from the leafs nodes to the root node...
	while(GetNodeCount() > 1)
		CombineTwoLowestFreqNodes(); // Combine the two lowest frequency elements until there is only one node left
}

// Return a pointer to the first base node in the tree...
TreeStruct* CPriorityQueue::GetTreeRootNode()
{
	TreeStruct *pNode = GetBaseTreeNode(0);
	if(pNode == NULL){return NULL;} // This shouln't happen
	while(pNode->Parent != NULL)
		pNode = pNode->Parent;
	
	return pNode;
}

// Desallocate our huffman tree memory
void CPriorityQueue::KillTree()
{
	//if(GetNodeCount() < 1){return;}
	TreeStruct *pNode = GetTreeRootNode();
	if(pNode == NULL){return;}

	// Delete the left and right node recursivelly...
	KillTreeNode(pNode);
}

// Desallocate our huffman tree memory(recursivelly)
void CPriorityQueue::KillTreeNode(TreeStruct *pNode)
{
	if(pNode == NULL)
		return;

	// Call this very same funciton recursivelly with the childs nodes of the current node...
	KillTreeNode(pNode->LeftChild);
	KillTreeNode(pNode->RightChild);

	// We don't want to delete a leaf node...	
	SAFE_DELETE_OBJECT(pNode); // Delete the current node
}


// Return a pointer to the first base node in the tree...
TreeStruct* GetBaseTreeNode(BYTE Indx)
{
	TreeBaseNodeListStruct *pBaseNode = pTreeLeafsNode->Child;
	TreeStruct *pTreeBase = &pBaseNode->NodeDat;

	// Find the next root node
	for(BYTE Cpt = 0; Cpt < Indx; Cpt++){
		pBaseNode = pBaseNode->Child;
		pTreeBase = &pBaseNode->NodeDat;
	}	
	
	return pTreeBase;
}

DWORD CPriorityQueue::GenBinCodes(BinCodeStruct *pBinCodes)
{	
	DWORD NumEncodedChar = 0;

	// Get the first leaf nodes of the tree
	int BaseNodeIndx = 0;

	TreeStruct *pTreeNode = GetBaseTreeNode(BaseNodeIndx);

	// Loop throught all the leaf nodes to find there respective binary code
	while(pTreeNode != NULL){


		// Save bin code info
		WORD BinCode = 0;
		BYTE BitIndx = 0;

		BYTE CharToEncode = pTreeNode->Char;
		pBinCodes[CharToEncode].Frequency = pTreeNode->Frequency;

		// Climb up the tree, updating the bin code
		while(pTreeNode != NULL){

			// Update the bin code for this node, if this isn't the root of the tree
			if(pTreeNode->NodeType != ROOT_NODE && pTreeNode->BinaryCode != 0){
				if(BitIndx <= 0)
					BinCode = pTreeNode->BinaryCode;
				else
					BinCode = BinCode | (1 << BitIndx);
			}
			BitIndx++;

			// Point to the node's parent
			pTreeNode = pTreeNode->Parent;
		}

		// Save the binary code in the given array
		pBinCodes[CharToEncode].BinCode   = BinCode;
		pBinCodes[CharToEncode].BitsCount = BitIndx - 1;

		// This will tell us later how many char have been encoded
		NumEncodedChar++;

		// Set the leaf node pointer to the next leaf node
		BaseNodeIndx++;
		pTreeNode = GetBaseTreeNode(BaseNodeIndx);
		if(BaseNodeIndx >= 256)
			break;
	}

	return NumEncodedChar;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

CBitStream::CBitStream()
{
	pBuffer = NULL;
	IsAllocated = false;
	AllocateExtraDWORD = false;

	BufSize = 0;
	BufIndx = 0;
	BitIndx = 0;
}

CBitStream::~CBitStream()
{
	this->Free();
}

////////////////////////////////////////////////////////////////////////////////////////////

void CBitStream::Allocate(DWORD Size)
{
	if(IsAllocated)
		this->Free();

	BufSize = Size;
	pBuffer = new BYTE[BufSize];
	IsAllocated = true;
	this->Erase();
}

////////////////////////////////////////////////////////////////////////////////////////////

void CBitStream::Erase()
{
	if(IsAllocated){
		ZeroMemory(&pBuffer[0], BufSize);

		BufIndx = 0;
		BitIndx = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CBitStream::Fill(BYTE* pBuf)
{
	if(IsAllocated){
		Erase();
		memcpy(&pBuffer[0], &pBuf[0], BufSize);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CBitStream::Free()
{
	if(IsAllocated){
		SAFE_DELETE_ARRAY(pBuffer);
		IsAllocated = false;

		BufSize = 0;
		BufIndx = 0;
		BitIndx = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CBitStream::UpdateCounters(BYTE BitsProcessed)
{
	BitIndx += BitsProcessed;
	while(BitIndx >= 8){
		BitIndx -= 8; 
		BufIndx++;	
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

void CBitStream::Save(FILE *OutF)
{
	fwrite(&pBuffer[0], 1, BufSize, OutF);
}

////////////////////////////////////////////////////////////////////////////////////////////

void CBitStream::EncodeNextChar(WORD wBinCode, BYTE bBitsCount)
{
	// Encode the bincode bits per bits
	for(int Cpt = bBitsCount-1; Cpt >= 0; Cpt--){
	
		// Test the bincode bits and replicate them in our bits stream
		if(TestBit(wBinCode, Cpt))
			SetBit(&pBuffer[BufIndx], 7-BitIndx);
	
		// Update the counters
		UpdateCounters(1);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

BYTE CBitStream::DecodeNextChar(TreeStruct *pRootNode)
{
	// Get the huffman tree root node pointer
	TreeStruct *pNode = pRootNode;

	// Decode the data stream bits per bits
	while(1){
	
		// Test the current bit, then climb down the tree until we're done
		switch(TestBit(pBuffer[BufIndx], 7-BitIndx))
		{
		case true:  if(pNode->RightChild != NULL){pNode = pNode->RightChild;} break;
		case false: if(pNode->LeftChild  != NULL){pNode = pNode->LeftChild;}  break;
		}
		
		// Update the counters
		UpdateCounters(1);

		// Return the decoded char if we hit the leaf node
		if(pNode->NodeType == LEAF_NODE)
			return pNode->Char; 
	}
}


