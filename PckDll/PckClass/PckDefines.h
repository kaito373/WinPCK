//////////////////////////////////////////////////////////////////////
// PckDefines.h: 用于解析完美世界公司的pck文件中的数据，并显示在List中
// 头文件
//
// 此程序由 李秋枫/stsm/liqf 编写
//
// 此代码预计将会开源，任何基于此代码的修改发布请保留原作者信息
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
#define PCK_MSG_USERCANCELED	1   /* 用户取消 */
#define PCK_ERROR				2   /* Generic error */
#define PCK_ERR_DISKFULL		3	/* 磁盘已满 */	//磁盘空间不足，申请空间：%d，剩余空间：%d
#define PCK_ERR_VIEWMAP_FAIL	4	/* Mapping 失败 */
#define PCK_ERR_VIEW			5	/* View 失败 */
#define PCK_ERR_OPENMAPVIEWR	6	/* OpenMappingViewAllRead 失败 */
#define PCK_ERR_MALLOC			7	/* 内存申请失败 */

/* end-of-error-codes */

//日志

#define LOG_IMAGE_NOTICE				0
#define LOG_IMAGE_INFO					1
#define LOG_IMAGE_WARNING				2
#define LOG_IMAGE_ERROR					3
#define LOG_IMAGE_DEBUG					4
#define LOG_IMAGE_EMPTY					5
#define LOG_IMAGE_COUNT					6

//数字转字符时使用的字符串长度
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
#define TEXT_LOG_CLOSEFILE				"Закрытие файл"
#define	TEXT_LOG_FLUSH_CACHE			"Запись в кеш..."
#define	TEXT_LOG_RESTOR_OK				"Восстановление данных успешно"

#define TEXT_LOG_WORKING_DONE			"Операция завершена"

#define	TEXT_LOG_LEVEL_THREAD			"Уровень сжатия=%d"

#define	TEXT_LOG_UPDATE_ADD				"Режим обновления"
#define	TEXT_LOG_UPDATE_NEW				"Новый режим"

#define	TEXT_LOG_RENAME					"Переименовние/удаление файлов..."
#define	TEXT_LOG_REBUILD				"Восстановить файлы PCK..."
#define	TEXT_LOG_RECOMPRESS				"Перепаковка файлы PCK..."


#define	TEXT_LOG_CREATE					"Создать новый файл PCK:%s..."

#define	TEXT_LOG_COMPRESSOK				"Сжатие завершено, запись индекса..."

#define	TEXT_LOG_EXTRACT				"Распакуйте файл..."



//ERROR STRING
#define	TEXT_ERROR						"ошибка"

#define TEXT_INVALID_VERSION			"Неверный идентификатор версии！"

#define	TEXT_MALLOC_FAIL				"Не удалось подать заявку на память!"
#define	TEXT_CREATEMAP_FAIL				"Не удалось создать сопоставление файлов!"
#define	TEXT_CREATEMAPNAME_FAIL			"Картографический файл\"%s\"Ошибка!"
#define	TEXT_OPENNAME_FAIL				"Открыть файл\"%s\"Ошибка!"
#define	TEXT_VIEWMAP_FAIL				"Не удалось создать вид карты!"
#define	TEXT_VIEWMAPNAME_FAIL			"файл\"%s\"Не удалось создать вид карты!"

#define TEXT_READ_INDEX_FAIL			"Ошибка чтения таблицы индекса файла!"
#define TEXT_UNKNOWN_PCK_FILE			"Нераспознанный файл PCK!"

#define	TEXT_OPENWRITENAME_FAIL			"Открыть запись файла\"%s\"Ошибка!"
#define	TEXT_READFILE_FAIL				"Ошибка чтения файла!"
#define	TEXT_WRITEFILE_FAIL				"Ошибка записи файла!"
#define TEXT_WRITE_PCK_INDEX			"Ошибка записи в индекс файла!"

#define	TEXT_USERCANCLE					"Пользователь отменил выход!"

#define	TEXT_COMPFILE_TOOBIG			"Сжатый файл слишком большой!"
#define	TEXT_UNCOMP_FAIL				"Распаковка файла не удалась!"

#define	TEXT_UNCOMPRESSDATA_FAIL		"файл %s \r\nОшибка распаковки данных!"
#define	TEXT_UNCOMPRESSDATA_FAIL_REASON	"Ошибка распаковки данных: %s"

#define	TEXT_ERROR_OPEN_AFTER_UPDATE	"Сбой при открытии, возможно, потому, что последняя операция вызвала повреждение файла.\r\nВернуться к последнему открытому файлу?"
#define	TEXT_ERROR_GET_RESTORE_DATA		"Ошибка при получении информации о восстановлении"
#define	TEXT_ERROR_RESTORING			"Ошибка при восстановлении"
#define	TEXT_ERROR_DUP_FOLDER_FILE		"Exist (имя файла = имя папки), выход."

#define TEXT_UPDATE_FILE_INFO			"Количество предварительно добавленных файлов=%d, Ожидаемый размер файла=%lld."

#define TEXT_PCK_SIZE_INVALID			"Неверный размер файла PCK"
#define TEXT_VERSION_NOT_FOUND			"Не нашел верную версию"


#define	TEXT_EVENT_WRITE_PCK_DATA_FINISH			"WPckDataF%d"

#endif