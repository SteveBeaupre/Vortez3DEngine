#include "Encryptor.h"

CEncryptor::CEncryptor()
{
	UseStrongEncryption = false;
	NumKeyChars = 0;
	pKeyCharRNG = NULL;
}

CEncryptor::~CEncryptor()
{
	SAFE_DELETE_ARRAY(pKeyCharRNG);
}

void CEncryptor::Initialize(void *pKey, UINT KeyLen, bool StrongEncryption)
{
	CCRC32 crc;

	BYTE *k = (BYTE*)pKey;
	NumKeyChars = KeyLen;
	
	int s = crc.Generate(k, KeyLen);
	KeyRNG.InitializeGenerator(s);

	SAFE_DELETE_ARRAY(pKeyCharRNG);

	if(StrongEncryption){
		UseStrongEncryption = StrongEncryption;

		pKeyCharRNG = new CRNG[KeyLen];

		for(DWORD i = 0; i < KeyLen; i++){
			s = crc.Generate(&k[i], 1);
			pKeyCharRNG[i].InitializeGenerator(s);
		}
	}
}

void CEncryptor::GenKey(BYTE *pKey, DWORD KeySize, CRNG *rng)
{
	for(DWORD i = 0; i < KeySize; i++)
		pKey[i] = (BYTE)(rng->ExtractNumber() & 0x000000FF);
}

void CEncryptor::XOR(BYTE *pBlock, BYTE *pKey, DWORD BlockSize)
{
	for(DWORD i = 0; i < BlockSize; i++)
		pBlock[i] ^= pKey[i];
}

void CEncryptor::Encrypt(void *pBuffer, DWORD BufferSize)
{
	BYTE *buf = (BYTE*)pBuffer;

	int BlockSize = 4;
	int NumBytesLeft = BufferSize;

	BYTE *pKey = new BYTE[BlockSize];

	while(NumBytesLeft > 0){

		ZeroMemory(pKey, BlockSize);

		if(NumBytesLeft < BlockSize)
			BlockSize = NumBytesLeft;

		GenKey(pKey, BlockSize, &KeyRNG);
		XOR(buf, pKey, BlockSize);

		if(UseStrongEncryption){
			for(int i = 0; i < NumKeyChars; i++){
				GenKey(pKey, BlockSize, &pKeyCharRNG[i]);
				XOR(buf, pKey, BlockSize);
			}
		}

		buf += BlockSize;
		NumBytesLeft -= BlockSize;
	}

	SAFE_DELETE_ARRAY(pKey);
}

void CEncryptor::Decrypt(void *pBuffer, DWORD BufferSize)
{
	Encrypt(pBuffer, BufferSize);
}
