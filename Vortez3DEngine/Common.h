//-----------------------General----------------------------------------//
#define OPENGL_RENDERER     0
#define DIRECTX9_RENDERER   1
//------------------------Mouse-----------------------------------------//
//These macro will be used for describe which of the 3 button of the mouse have been pressed
#define LEFT_MOUSE_BUTTON    0
#define MIDDLE_MOUSE_BUTTON  1
#define RIGHT_MOUSE_BUTTON   2
//-------------------------Fonts------------------------------------------//
#define FONT_BASE_DEFAULT    10000
//-------------------------CModel-----------------------------------------//
//BitFields for va's settings
#define VA_VERTEX      0x00000001
#define VA_COLOR       0x00000002
#define VA_TEXTURE     0x00000004
#define VA_AUXTEXTURE  0x00000008
#define VA_NORMALS     0x00000010
#define VA_FOG         0x00000020
//////////////////////////////////////////
#define VA_VC          (VA_VERTEX | VA_COLOR)
#define VA_VT          (VA_VERTEX | VA_TEXTURE)
#define VA_VN          (VA_VERTEX | VA_NORMALS)
#define VA_VCN         (VA_VERTEX | VA_COLOR | VA_NORMALS)
#define VA_VTN         (VA_VERTEX | VA_TEXTURE | VA_NORMALS)
#define VA_VTNF        (VA_VERTEX | VA_TEXTURE | VA_NORMALS | VA_FOG)
#define VA_VCTN        (VA_VERTEX | VA_COLOR | VA_TEXTURE | VA_NORMALS)
#define VA_VCTNF       (VA_VERTEX | VA_COLOR | VA_TEXTURE | VA_NORMALS | VA_FOG)
//////////////////////////////////////////
#define VA_ALL         (VA_VERTEX | VA_COLOR | VA_TEXTURE | VA_AUXTEXTURE | VA_NORMALS | VA_FOG)
#define VA_NONE        0x00000000
//////////////////////////////////////////
#define CREATE_ARRAY_MACRO(buf, type, count)  buf = new type[(count)];
#define CREATE_OPTIONAL_ARRAY_MACRO(name, buf, type, count) if(pModel->AvailableArrays[(name)] != 0){buf = new type[(count)];}
//////////////////////////////////////////
#define VA_VERTEX_ARRAY   0
#define VA_COLOR_ARRAY    1
#define VA_TEX1_ARRAY     2
#define VA_TEX2_ARRAY     3
#define VA_TEX3_ARRAY     4
#define VA_TEX4_ARRAY     5
#define VA_NORMAL_ARRAY   6
#define VA_FOG_ARRAY      7
//////////////////////////////////////////
#define SIGNATURE        "Vortez 3D Models"
#define VERSION_NUMBER   "Version #: v2.00"
//--------------------Sound/Tunes Stuff-----------------------------------//
#define WM_PLAY_SOUNDFX          WM_USER + 110
#define WM_PLAY_TUNE             WM_USER + 111
//----------------------Network Stuff-------------------------------------//
#define ON_WINSOCK_MESSAGE       WM_USER + 120
#define ON_NETWORK_MESSAGE       WM_USER + 121

#define MSG_LOGIN_REQUEST   0x00010000
#define MSG_LOGIN_DATA      0x00020000
#define MSG_LOGIN_REPLY     0x00030000

#define MSG_ON_PLAYER_LOGED_IN   0x00040000
#define MSG_ON_PLAYER_LOGED_OFF  0x00050000
#define MSG_EVERYONE_READY       0x00060000

#define MSG_ON_CHAT_MSG          0x00070000

#define ERR_SERVER_FULL              1
#define ERR_INVALID_SOCKET           2
#define ERR_SOCKET_ALREADY_IN_USE    3

#define ERR_LOGIN_PASS_INVALID       1
#define ERR_PLAYER_NAME_IN_USE       2
#define ERR_VERSION_DONT_MATCH       3

#define ALL_PLAYERS  0xFFFFFFFF

/*#define PLAYER1   0x00000001
#define PLAYER2   0x00000002
#define PLAYER3   0x00000004
#define PLAYER4   0x00000008
#define PLAYER5   0x00000010
#define PLAYER6   0x00000020
#define PLAYER7   0x00000040
#define PLAYER8   0x00000080
#define PLAYER9   0x00000100
#define PLAYER10  0x00000200
#define PLAYER11  0x00000400
#define PLAYER12  0x00000800
#define PLAYER13  0x00001000
#define PLAYER14  0x00002000
#define PLAYER15  0x00004000
#define PLAYER16  0x00008000
#define PLAYER17  0x00010000
#define PLAYER18  0x00020000
#define PLAYER19  0x00040000
#define PLAYER20  0x00080000
#define PLAYER21  0x00100000
#define PLAYER22  0x00200000
#define PLAYER23  0x00400000
#define PLAYER24  0x00800000
#define PLAYER25  0x01000000
#define PLAYER26  0x02000000
#define PLAYER27  0x04000000
#define PLAYER28  0x08000000
#define PLAYER29  0x10000000
#define PLAYER30  0x20000000
#define PLAYER31  0x40000000
#define PLAYER32  0x80000000*/


// Client/Server
/*#define ON_CUSTOM_MESSAGE       0
#define ON_CONN_ACCEPTED        1
#define ON_CONN_REFUSED         2
// Server
#define ON_CLIENT_        1
#define ON_LOGIN_REQUEST        3
// Client
#define ON_LOGIN_SUCCESS        4
#define ON_LOGIN_FAILED         5*/
