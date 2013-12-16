unit VortezSDK;
interface

const
  CRC32_INIT = 0;
  CRC32_FEED = 1;
  CRC32_GET  = 2;

  FILE_BEGIN   = 0;
  FILE_CURRENT = 1;
  FILE_END     = 2;

  ABORT_IF_RUNNING = 0;
  WAIT_IF_RUNNING  = 1;
  WAIT_FOR_FINISH  = 2;

Type
  TFileAttribs = record
    ReadOnly: Boolean;
    Hidden: Boolean;
    System: Boolean;
    Directory: Boolean;
    Archive: Boolean;
    Normal: Boolean;
    Temporary: Boolean;
    SparseFile: Boolean;
    ReparsePoint: Boolean;
    Compressed: Boolean;
    Offline: Boolean;
    NotIndexed: Boolean;
    Encrypted: Boolean;
    IsVirtual: Boolean;
  end;

Type
  TBitArray = class
    function  IsAllocated(): Integer; virtual; cdecl; abstract;
    procedure Allocate(NumBits: Integer); virtual; cdecl; abstract;
    procedure Resize(NumBits: Integer); virtual; cdecl; abstract;
    procedure FreeMem(); virtual; cdecl; abstract;

    procedure ClearAll(); virtual; cdecl; abstract;
    procedure SetAll(); virtual; cdecl; abstract;

    function  GetBit(Indx: Integer): Boolean; virtual; cdecl; abstract;
    procedure SetBit(Indx: Integer; Value: Boolean); virtual; cdecl; abstract;

    procedure Free; virtual; cdecl; abstract;
  end;

  TCRC32 = class
    function  Generate(pBlock: Pointer; BlockLen: Cardinal): Cardinal; virtual; cdecl; abstract;
    function  GenerateExt(Op: Cardinal; pBlock: Pointer; BlockLen: Cardinal): Cardinal; virtual; cdecl; abstract;

    procedure Free; virtual; cdecl; abstract;
  end;

  TDriveIO = class
    function  IsOpened(): Boolean; virtual; cdecl; abstract;
    function  Open(Drive: Char; WriteAccess: Boolean): Boolean; virtual; cdecl; abstract;
    procedure Close(); virtual; cdecl; abstract;

    function  Tell(): Int64; virtual; cdecl; abstract;
    procedure Seek(uiDistanceToMove: Int64; dwMoveMethod: LongWord = FILE_BEGIN); virtual; cdecl; abstract;

    function  ReadBuffer(pBuffer: Pointer; NumBytesToRead: Cardinal): LongWord; virtual; cdecl; abstract;
    function  ReadSectors(pBuffer: Pointer; NumSectorsToRead: Cardinal): LongWord; virtual; cdecl; abstract;
    function  WriteBuffer(pBuffer: Pointer; NumBytesToWrite: Cardinal): LongWord; virtual; cdecl; abstract;
    function  WriteSectors(pBuffer: Pointer; NumSectorsToWrite: Cardinal): LongWord; virtual; cdecl; abstract;

    function  GetOpenedDriveLetter: Char; virtual; cdecl; abstract;

    function  GetSize: Int64; virtual; cdecl; abstract;
    function  GetSectorsCount: Int64; virtual; cdecl; abstract;

    function  GetNumberOfClusters: LongWord; virtual; cdecl; abstract;
    function  GetNumBytesPerSector: LongWord; virtual; cdecl; abstract;
    function  GetNumSectorsPerCluster: LongWord; virtual; cdecl; abstract;

    function  GetNumBytesFree(Drive: Char): Int64; virtual; cdecl; abstract;
    function  GetNumBytesUsed(Drive: Char): Int64; virtual; cdecl; abstract;
    function  GetNumSectorFree(Drive: Char): Int64; virtual; cdecl; abstract;
    function  GetNumSectorUsed(Drive: Char): Int64; virtual; cdecl; abstract;

    procedure Free; virtual; cdecl; abstract;
  end;

  TFileIO = class
    function  IsOpened(): Boolean; virtual; cdecl; abstract;
    function  OpenForReading(fname: PCHAR; CanWrite: Boolean = False): Boolean; virtual; cdecl; abstract;
    function  OpenForWriting(fname: PCHAR; CanRead: Boolean = False): Boolean; virtual; cdecl; abstract;
    function  OpenForAppending(fname: PCHAR; CanRead: Boolean = False): Boolean; virtual; cdecl; abstract;
    procedure Close(); virtual; cdecl; abstract;

    function  GetSize(): Int64; virtual; cdecl; abstract;
    procedure Flush(); virtual; cdecl; abstract;

    function  Tell(): Int64; virtual; cdecl; abstract;
    procedure Seek(uiDistanceToMove: Int64; dwMoveMethod: LongWord = FILE_BEGIN); virtual; cdecl; abstract;

    function  Read(pBuffer: Pointer; NumBytesToRead: LongWord): LongWord; virtual; cdecl; abstract;
    function  Write(pBuffer: Pointer; NumBytesToWrite: LongWord): LongWord; virtual; cdecl; abstract;

    procedure Free; virtual; cdecl; abstract;
  end;

  TFileManager = class
    function  DoesExist(pFileName: PCHAR): Boolean; virtual; cdecl; abstract;
    procedure Copy(pSourceFileName: PCHAR; pDestFileName: PCHAR; FailIfExist: Boolean); virtual; cdecl; abstract;
    procedure Move(pSourceFileName: PCHAR; pDestFileName: PCHAR); virtual; cdecl; abstract;
    procedure Delete(pFileName: PCHAR); virtual; cdecl; abstract;
    procedure Rename(pOldFileName: PCHAR; pNewFileName: PCHAR); virtual; cdecl; abstract;
    function  SendToRecycleBin(pFileName: PCHAR; Flags: WORD = $0054): Integer; virtual; cdecl; abstract;
    function  IsInUse(pFileName: PCHAR): Boolean; virtual; cdecl; abstract;
    function  GetSize(pFileName: PCHAR): LongWord; virtual; cdecl; abstract;
    function  GetSize64(pFileName: PCHAR): Int64; virtual; cdecl; abstract;
    procedure GetShortName(pLongFileName: PCHAR; pShortFileName: PCHAR); virtual; cdecl; abstract;
    function  GetAttributes(pFileName: PCHAR; pFileAttribs: TFileAttribs): LongWord; virtual; cdecl; abstract;
    function  SetAttributes(pFileName: PCHAR; pFileAttribs: TFileAttribs): Boolean; virtual; cdecl; abstract;
    function  SetAttributes32(pFileName: PCHAR; Attribs: LongWord): Boolean; virtual; cdecl; abstract;
    procedure Open(pFileName: PCHAR; pParams: PCHAR); virtual; cdecl; abstract;
    procedure OpenURL(pURL: PCHAR); virtual; cdecl; abstract;

    procedure Free; virtual; cdecl; abstract;
  end;

  TFolderManager = class
    function  Create(pFolderName: PCHAR): Boolean; virtual; cdecl; abstract;
    function  Delete(pFolderName: PCHAR): Boolean; virtual; cdecl; abstract;
    function  DeleteTree(pFolderName: PCHAR; Level: LongWord = 0): Boolean; virtual; cdecl; abstract;
    procedure Rename(pOldName: PCHAR; pNewName: PCHAR); virtual; cdecl; abstract;
    procedure SendToRecycleBin(pFileName: PCHAR); virtual; cdecl; abstract;
    function  DoesExist(pFolderName: PCHAR): Boolean; virtual; cdecl; abstract;
    procedure GetCurrent(pFolderName: PCHAR); virtual; cdecl; abstract;
    procedure SetCurrent(pFolderName: PCHAR); virtual; cdecl; abstract;
    procedure GetShortName(pLongFileName: PCHAR; pShortFileName: PCHAR); virtual; cdecl; abstract;

    procedure Free; virtual; cdecl; abstract;
  end;

  TDriveManager = class
    function  DoesExist(Drive: CHAR): Boolean; virtual; cdecl; abstract;
    function  GetAvailableDrives(): LongWord; virtual; cdecl; abstract;

    function  FreeSpace(Drive: CHAR): Int64; virtual; cdecl; abstract;
    function  UsedSpace(Drive: CHAR): Int64; virtual; cdecl; abstract;
    function  GetSize(Drive: CHAR): Int64; virtual; cdecl; abstract;
    function  GetNumSectors(Drive: CHAR): Int64; virtual; cdecl; abstract;
    function  GetNumBytesPerSector(Drive: CHAR): LongWord; virtual; cdecl; abstract;

    function  GetType(Drive: CHAR): LongWord; virtual; cdecl; abstract;
    function  IsCDRom(Drive: CHAR): Boolean; virtual; cdecl; abstract;
    function  IsHardDriveDevice(Drive: CHAR): Boolean; virtual; cdecl; abstract;
    function  IsFlashDriveDevice(Drive: CHAR): Boolean; virtual; cdecl; abstract;
    function  IsDriveRemovable(Drive: CHAR): Boolean; virtual; cdecl; abstract;
    function  IsRemoteDrive(Drive: CHAR): Boolean; virtual; cdecl; abstract;
    function  IsRamDisk(Drive: CHAR): Boolean; virtual; cdecl; abstract;

    function  Information(Drive: CHAR; pLabel: PCHAR; pSerialNumber: PCHAR; pFileSystem: PCHAR): Boolean; virtual; cdecl; abstract;

    procedure OpenCDRom(Drive: CHAR; bOpenDrive: Boolean); virtual; cdecl; abstract;

    procedure Free; virtual; cdecl; abstract;
  end;

  TEncryptor = class
    procedure Initialize(pKey: Pointer; StrongEncryption: Boolean = False); virtual; cdecl; abstract;

    procedure Encrypt(pBuffer: Pointer; BufferSize: Cardinal); virtual; cdecl; abstract;
    procedure Decrypt(pBuffer: Pointer; BufferSize: Cardinal); virtual; cdecl; abstract;

    procedure Free; virtual; cdecl; abstract;
  end;

  TRNG = class
    procedure InitializeGenerator(Seed: Integer); virtual; cdecl; abstract;
    function  ExtractNumber(): Integer; virtual; cdecl; abstract;

    procedure Free; virtual; cdecl; abstract;
  end;

  TRawBuffer = class
    function  IsAllocated(): Boolean; virtual; cdecl; abstract;

    function  GetBuffer(Indx: LongWord = 0): PBYTE; virtual; cdecl; abstract;
    function  GetStrBuffer(Indx: LongWord = 0): PCHAR; virtual; cdecl; abstract;

    function  GetBufferSize(): LongWord; virtual; cdecl; abstract;
    function  GetStrBufferSize(Indx: LongWord = 0): LongWord; virtual; cdecl; abstract;

    function  AllocateBuffer(Size: LongWord): Boolean; virtual; cdecl; abstract;
    procedure EraseBuffer(); virtual; cdecl; abstract;
    procedure SetBufferBytes(Val: Byte); virtual; cdecl; abstract;
    procedure FreeBuffer(); virtual; cdecl; abstract;

    procedure Free; virtual; cdecl; abstract;
  end;

  TThread = class
    function  GetThreadHandle(): THandle; virtual; cdecl; abstract;

    function  StartThread(lpStartAddress: Pointer; lpParameter: Pointer; szEventName: PCHAR; WaitMode: LongWord = ABORT_IF_RUNNING): Boolean; virtual; cdecl; abstract;
    procedure StopThread(); virtual; cdecl; abstract;

    function  IsThreadRunning(): Boolean; virtual; cdecl; abstract;
    procedure WaitForThread(); virtual; cdecl; abstract;

    function  MustExitThread(): Boolean; virtual; cdecl; abstract;

    procedure Free; virtual; cdecl; abstract;
  end;

  TPrecisionTimer = class
    procedure Reset(); virtual; cdecl; abstract;
    function  Tick(): Single; virtual; cdecl; abstract;
    procedure Free; virtual; cdecl; abstract;
  end;

  TPrecisionTimer64 = class
    procedure Reset(); virtual; cdecl; abstract;
    function  Tick(): Double; virtual; cdecl; abstract;
    procedure Free; virtual; cdecl; abstract;
  end;

  TGameTimer = class
    procedure Reset(); virtual; cdecl; abstract;
    function  Tick(): Single; virtual; cdecl; abstract;
    function  GetFPS(): Single; virtual; cdecl; abstract;
    procedure Free; virtual; cdecl; abstract;
  end;

  TCycleTimer = class
    procedure Start(); virtual; cdecl; abstract;
    function  Stop(): Int64; virtual; cdecl; abstract;
    procedure Free; virtual; cdecl; abstract;
  end;

  TWindow = class
    procedure GetClsName(pClassName: PCHAR; BufferSize: LongWord); virtual; cdecl; abstract;
    procedure GetText(pCaption: PCHAR; BufferSize: LongWord); virtual; cdecl; abstract;
    procedure GetScreenPos(x: PLongWord; y: PLongWord); virtual; cdecl; abstract;
    procedure GetClientPos(x: PLongWord; y: PLongWord); virtual; cdecl; abstract;
    procedure GetSize(w: PLongWord; h: PLongWord); virtual; cdecl; abstract;
    procedure GetStyles(pStyles: PLongWord); virtual; cdecl; abstract;

    procedure SetText(pCaption: PCHAR); virtual; cdecl; abstract;
    procedure SetScreenPos(x: LongWord; y: LongWord); virtual; cdecl; abstract;
    procedure SetClientPos(x: LongWord; y: LongWord); virtual; cdecl; abstract;
    procedure SetSize(w: LongWord; h: LongWord); virtual; cdecl; abstract;
    procedure SetStyles(Styles: LongWord); virtual; cdecl; abstract;

    procedure Hide(); virtual; cdecl; abstract;
    procedure Show(); virtual; cdecl; abstract;
    procedure ShowDefault(); virtual; cdecl; abstract;
    procedure Maximize(); virtual; cdecl; abstract;
    procedure Minimize(); virtual; cdecl; abstract;
    procedure Restore(); virtual; cdecl; abstract;

    function GetParentWnd(GetDesktopWindowOnFail: Boolean = False): Boolean; virtual; cdecl; abstract;
    function GetFirstChild(RestoreValueOnFail: Boolean = False): Boolean; virtual; cdecl; abstract;
    function GetLastChild(RestoreValueOnFail: Boolean = False): Boolean; virtual; cdecl; abstract;
    function GetNextChild(RestoreValueOnFail: Boolean = False): Boolean; virtual; cdecl; abstract;
    function GetPrevChild(RestoreValueOnFail: Boolean = False): Boolean; virtual; cdecl; abstract;
    function GetNumChilds(): LongWord; virtual; cdecl; abstract;

    function GetFromPoint(x: LongWord; y: LongWord): Boolean; virtual; cdecl; abstract;
    function CenterCursorOnWindow(): Boolean; virtual; cdecl; abstract;

    procedure PostMouseMoveMsg(x: LongWord; y: LongWord); virtual; cdecl; abstract;
    procedure PostMouseWheelMsg(DirUp: Boolean; NumRolls: Word; x: LongWord; y: LongWord); virtual; cdecl; abstract;
    procedure PostMouseLeftBtnDownMsg(x: LongWord; y: LongWord); virtual; cdecl; abstract;
    procedure PostMouseLeftBtnUpMsg(x: LongWord; y: LongWord); virtual; cdecl; abstract;
    procedure PostMouseMiddleBtnDownMsg(x: LongWord; y: LongWord); virtual; cdecl; abstract;
    procedure PostMouseMiddleBtnUpMsg(x: LongWord; y: LongWord); virtual; cdecl; abstract;
    procedure PostMouseRightBtnDownMsg(x: LongWord; y: LongWord); virtual; cdecl; abstract;
    procedure PostMouseRightBtnUpMsg(x: LongWord; y: LongWord); virtual; cdecl; abstract;

    procedure PostKeyDownMsg(VKCode: Byte); virtual; cdecl; abstract;
    procedure PostKeyUpMsg(VKCode: Byte); virtual; cdecl; abstract;

    procedure Free; virtual; cdecl; abstract;
  end;

  function  ReadBit (pBitField: Pointer; BitIndx: LongWord; DataSize: LongWord = 0): Boolean; cdecl; external 'VortezSDKForDelphi.dll';
  procedure WriteBit(pBitField: Pointer; BitIndx: LongWord; BitValue: Byte = 1; DataSize: LongWord = 0);cdecl; external 'VortezSDKForDelphi.dll';

  function IntToBool(i: Integer): Boolean; cdecl; external 'VortezSDKForDelphi.dll';
  function BoolToInt(b: Boolean): Integer; cdecl; external 'VortezSDKForDelphi.dll';

  function DWORDToLARGE_INTEGER(dw: LongWord): Int64; cdecl; external 'VortezSDKForDelphi.dll';
  function DWORDToUI64(dw: LongWord): Int64; cdecl; external 'VortezSDKForDelphi.dll';

  function DWORDX2ToUI64(dwHi: LongWord; dwLo: LongWord): Int64; cdecl; external 'VortezSDKForDelphi.dll';
  function UI64ToLARGE_INTEGER(ui: Int64): Int64; cdecl; external 'VortezSDKForDelphi.dll';
  function LARGE_INTEGERToUI64(li: Int64): Int64; cdecl; external 'VortezSDKForDelphi.dll';

  function SectorsToBytes(NumSectors: Int64; SectorSize: LongWord): Int64; cdecl; external 'VortezSDKForDelphi.dll';
  function BytesToSectors(NumBytes: Int64; SectorSize: LongWord): Int64; cdecl; external 'VortezSDKForDelphi.dll';

  function LODWORD(qword: Int64): LongWord; cdecl; external 'VortezSDKForDelphi.dll';
  function HIDWORD(qword: Int64): LongWord; cdecl; external 'VortezSDKForDelphi.dll';

  function NewBitArray: TBitArray; stdcall; external 'VortezSDKForDelphi.dll';
  function NewCRC32: TCRC32; stdcall; external 'VortezSDKForDelphi.dll';
  function NewDriveIO: TDriveIO; stdcall; external 'VortezSDKForDelphi.dll';
  function NewFileIO: TFileIO; stdcall; external 'VortezSDKForDelphi.dll';
  function NewFileManager: TFileManager; stdcall; external 'VortezSDKForDelphi.dll';
  function NewFolderManager: TFolderManager; stdcall; external 'VortezSDKForDelphi.dll';
  function NewDriveManager: TDriveManager; stdcall; external 'VortezSDKForDelphi.dll';
  function NewEncryptor: TEncryptor; stdcall; external 'VortezSDKForDelphi.dll';
  function NewRNG: TRNG; stdcall; external 'VortezSDKForDelphi.dll';
  function NewRawBuffer: TRawBuffer; stdcall; external 'VortezSDKForDelphi.dll';
  function NewThread: TThread; stdcall; external 'VortezSDKForDelphi.dll';
  function NewTimer: TPrecisionTimer; stdcall; external 'VortezSDKForDelphi.dll';
  function NewTimer64: TPrecisionTimer64; stdcall; external 'VortezSDKForDelphi.dll';
  function NewGameTimer: TGameTimer; stdcall; external 'VortezSDKForDelphi.dll';
  function NewCycleTimer: TCycleTimer; stdcall; external 'VortezSDKForDelphi.dll';
  function NewWindow: TWindow; stdcall; external 'VortezSDKForDelphi.dll';
implementation
end.

