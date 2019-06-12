//////////////////////////////////////////////////////////////////////
// PckDefines.h: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2015.5.13
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <vector>
using namespace std;

typedef unsigned __int64	QWORD, *LPQWORD;


#if !defined(_PCKDEFINES_H_)
#define _PCKDEFINES_H_

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

#pragma warning ( disable : 4005 )
#pragma warning ( disable : 4995 )


typedef enum _FMTPCK
{
	FMTPCK_PCK = 0,
	FMTPCK_ZUP = 1,
	FMTPCK_CUP = 2,
	FMTPCK_UNKNOWN = 0x7fffffff
}FMTPCK;

typedef enum _PCKVER
{
	PCK_V2020,
	PCK_V2030,
	PCK_VXAJH
}PCK_CATEGORY;

typedef void(*ShowLogA)(const int log_level, const char *str);
typedef void(*ShowLogW)(const int log_level, const wchar_t *str);

#define MAX_PATH			260

//zlib
#define Z_OK				0

//params
#define	MT_MAX_MEMORY				0x80000000	//2048MB
#define MAX_COMPRESS_LEVEL			12
#define Z_DEFAULT_COMPRESS_LEVEL	9

#define PCK_OK					0   /* Successful result */
/* beginning-of-error-codes */
#define PCK_MSG_USERCANCELED	1   /* �û�ȡ�� */
#define PCK_ERROR				2   /* Generic error */
#define PCK_ERR_DISKFULL		3	/* �������� */	//���̿ռ䲻�㣬����ռ䣺%d��ʣ��ռ䣺%d
#define PCK_ERR_VIEWMAP_FAIL	4	/* Mapping ʧ�� */
#define PCK_ERR_VIEW			5	/* View ʧ�� */
#define PCK_ERR_OPENMAPVIEWR	6	/* OpenMappingViewAllRead ʧ�� */
#define PCK_ERR_MALLOC			7	/* �ڴ�����ʧ�� */

/* end-of-error-codes */

//��־

#define LOG_IMAGE_NOTICE				0
#define LOG_IMAGE_INFO					1
#define LOG_IMAGE_WARNING				2
#define LOG_IMAGE_ERROR					3
#define LOG_IMAGE_DEBUG					4
#define LOG_IMAGE_EMPTY					5
#define LOG_IMAGE_COUNT					6

//����ת�ַ�ʱʹ�õ��ַ�������
#define CHAR_NUM_LEN 12

#define	MAX_INDEXTABLE_CLEARTEXT_LENGTH	0x120
#define MAX_TAIL_LENGTH					300

#define	PCK_BEGINCOMPRESS_SIZE			20
#define	MAX_PATH_PCK_256				256
#define	MAX_PATH_PCK_260				260
#define PCK_ADDITIONAL_INFO_SIZE		252
#define PCK_DATA_START_AT				12

#define PCK_ENTRY_TYPE_INDEX			1
#define PCK_ENTRY_TYPE_NODE				2
#define PCK_ENTRY_TYPE_FOLDER			4
#define PCK_ENTRY_TYPE_DOTDOT			8
#define PCK_ENTRY_TYPE_ROOT				16
#define PCK_ENTRY_TYPE_TAIL_INDEX		0x80000000

#define	PCK_ADDITIONAL_KEY				"Angelica File Package"
#define	PCK_ADDITIONAL_INFO				PCK_ADDITIONAL_KEY", Perfect World Co. Ltd. 2002~2008. All Rights Reserved."

//LOG INFO STRING
#define TEXT_LOG_OPENFILE				"%s"
#define TEXT_LOG_CLOSEFILE				"���ѧܧ���ڧ� ��ѧۧ�"
#define	TEXT_LOG_FLUSH_CACHE			"���ѧ�ڧ�� �� �ܧ֧�..."
#define	TEXT_LOG_RESTOR_OK				"�������ѧߧ�ӧݧ֧ߧڧ� �էѧߧߧ�� ����֧�ߧ�"

#define TEXT_LOG_WORKING_DONE			"����֧�ѧ�ڧ� �٧ѧӧ֧��֧ߧ�"

#define	TEXT_LOG_LEVEL_THREAD			"�����ӧ֧ߧ� ��اѧ�ڧ�=%d"

#define	TEXT_LOG_UPDATE_ADD				"���֧اڧ� ��ҧߧ�ӧݧ֧ߧڧ�"
#define	TEXT_LOG_UPDATE_NEW				"����ӧ�� ��֧اڧ�"

#define	TEXT_LOG_RENAME					"���֧�֧ڧާ֧ߧ�ӧߧڧ�/��էѧݧ֧ߧڧ� ��ѧۧݧ��..."
#define	TEXT_LOG_REBUILD				"�������ѧߧ�ӧڧ�� ��ѧۧݧ� PCK..."
#define	TEXT_LOG_RECOMPRESS				"���֧�֧�ѧܧ�ӧܧ� ��ѧۧݧ� PCK..."


#define	TEXT_LOG_CREATE					"����٧էѧ�� �ߧ�ӧ�� ��ѧۧ� PCK:%s..."

#define	TEXT_LOG_COMPRESSOK				"���اѧ�ڧ� �٧ѧӧ֧��֧ߧ�, �٧ѧ�ڧ�� �ڧߧէ֧ܧ��..."

#define	TEXT_LOG_EXTRACT				"���ѧ��ѧܧ�ۧ�� ��ѧۧ�..."



//ERROR STRING
#define	TEXT_ERROR						"���ڧҧܧ�"

#define TEXT_INVALID_VERSION			"���֧ӧ֧�ߧ�� �ڧէ֧ߧ�ڧ�ڧܧѧ��� �ӧ֧��ڧڣ�"

#define	TEXT_MALLOC_FAIL				"���� ��էѧݧ��� ���էѧ�� �٧ѧ�ӧܧ� �ߧ� ��ѧާ���!"
#define	TEXT_CREATEMAP_FAIL				"���� ��էѧݧ��� ���٧էѧ�� �������ѧӧݧ֧ߧڧ� ��ѧۧݧ��!"
#define	TEXT_CREATEMAPNAME_FAIL			"���ѧ���ԧ�ѧ�ڧ�֧�ܧڧ� ��ѧۧ�\"%s\"����ڧҧܧ�!"
#define	TEXT_OPENNAME_FAIL				"����ܧ���� ��ѧۧ�\"%s\"����ڧҧܧ�!"
#define	TEXT_VIEWMAP_FAIL				"���� ��էѧݧ��� ���٧էѧ�� �ӧڧ� �ܧѧ���!"
#define	TEXT_VIEWMAPNAME_FAIL			"��ѧۧ�\"%s\"���� ��էѧݧ��� ���٧էѧ�� �ӧڧ� �ܧѧ���!"

#define TEXT_READ_INDEX_FAIL			"����ڧҧܧ� ���֧ߧڧ� ��ѧҧݧڧ�� �ڧߧէ֧ܧ�� ��ѧۧݧ�!"
#define TEXT_UNKNOWN_PCK_FILE			"���֧�ѧ���٧ߧѧߧߧ�� ��ѧۧ� PCK!"

#define	TEXT_OPENWRITENAME_FAIL			"����ܧ���� �٧ѧ�ڧ�� ��ѧۧݧ�\"%s\"����ڧҧܧ�!"
#define	TEXT_READFILE_FAIL				"����ڧҧܧ� ���֧ߧڧ� ��ѧۧݧ�!"
#define	TEXT_WRITEFILE_FAIL				"����ڧҧܧ� �٧ѧ�ڧ�� ��ѧۧݧ�!"
#define TEXT_WRITE_PCK_INDEX			"����ڧҧܧ� �٧ѧ�ڧ�� �� �ڧߧէ֧ܧ� ��ѧۧݧ�!"

#define	TEXT_USERCANCLE					"����ݧ�٧�ӧѧ�֧ݧ� ���ާ֧ߧڧ� �ӧ����!"

#define	TEXT_COMPFILE_TOOBIG			"���اѧ��� ��ѧۧ� ��ݧڧ�ܧ�� �ҧ�ݧ����!"
#define	TEXT_UNCOMP_FAIL				"���ѧ��ѧܧ�ӧܧ� ��ѧۧݧ� �ߧ� ��էѧݧѧ��!"

#define	TEXT_UNCOMPRESSDATA_FAIL		"��ѧۧ� %s \r\n����ڧҧܧ� ��ѧ��ѧܧ�ӧܧ� �էѧߧߧ��!"
#define	TEXT_UNCOMPRESSDATA_FAIL_REASON	"����ڧҧܧ� ��ѧ��ѧܧ�ӧܧ� �էѧߧߧ��: %s"

#define	TEXT_ERROR_OPEN_AFTER_UPDATE	"���ҧ�� ���� ���ܧ���ڧ�, �ӧ�٧ާ�اߧ�, �����ާ�, ���� ����ݧ֧էߧ�� ���֧�ѧ�ڧ� �ӧ�٧ӧѧݧ� ���ӧ�֧اէ֧ߧڧ� ��ѧۧݧ�.\r\n���֧�ߧ����� �� ����ݧ֧էߧ֧ާ� ���ܧ����ާ� ��ѧۧݧ�?"
#define	TEXT_ERROR_GET_RESTORE_DATA		"����ڧҧܧ� ���� ���ݧ��֧ߧڧ� �ڧߧ���ާѧ�ڧ� �� �ӧ����ѧߧ�ӧݧ֧ߧڧ�"
#define	TEXT_ERROR_RESTORING			"����ڧҧܧ� ���� �ӧ����ѧߧ�ӧݧ֧ߧڧ�"
#define	TEXT_ERROR_DUP_FOLDER_FILE		"Exist (�ڧާ� ��ѧۧݧ� = �ڧާ� ��ѧ�ܧ�), �ӧ����."

#define TEXT_UPDATE_FILE_INFO			"����ݧڧ�֧��ӧ� ���֧էӧѧ�ڧ�֧ݧ�ߧ� �է�ҧѧӧݧ֧ߧߧ�� ��ѧۧݧ��=%d, ���اڧէѧ֧ާ�� ��ѧ٧ާ֧� ��ѧۧݧ�=%lld."

#define TEXT_PCK_SIZE_INVALID			"���֧ӧ֧�ߧ�� ��ѧ٧ާ֧� ��ѧۧݧ� PCK"
#define TEXT_VERSION_NOT_FOUND			"���� �ߧѧ�֧� �ӧ֧�ߧ�� �ӧ֧��ڧ�"


#define	TEXT_EVENT_WRITE_PCK_DATA_FINISH			"WPckDataF%d"

#endif