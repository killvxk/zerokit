#ifndef __ZFS_PROXY_H_
#define __ZFS_PROXY_H_

/** ������� ��������� (������-����):
    * ��������� ���������� ������ ����� ��������� � ������-����� (�������� ARC4 � 48-������� ������).
    * ��������� ������ ������� ������ UNIX-��������.
    * ������ ������ �������� ��������.
    * ����������� ���������� ����� ����� ����� 32 �������� ������ 11.
    * ��������� ������� ��� ��������� ������� � ���� ��������/���������/������� � ����� (zfs_get_file_time).
    * ��������� ������� ��� ��������� ������ ������ (zfs_get_version).
    * ��������� ������� ��� ��������� ������� ����� (zfs_get_file_size).
    * ��������� ������� ��� ��������� ����������� ������� ����� �� �������� ��������� (zfs_set_end_of_file).
    * ��������� ����������� ��������� ��� ������ � �������� zfs_get_file_time.
    * ������� ����������� � ����������� ZFS_isERR -> ((x) & ZFS_ERRFLAG).
    * ������������� ��������� ������� �������.
    * ��������� ����������� ��� zfs_open (�������� flags).
    * ��������� ����� ������ ��������� ������.
    * ��������� ���� ������ ������������ � ��������.
    * � ������� zfs_write �������� �������������� �������� pWritten ��� �������� ������������ ������.
    * �������� ��������� ������� zfs_getc.
    * �������� ��������� ������� zfs_open.
    * �������� ��������� ������� zfs_get_time.
    * �������� ��������� ������� zfs_get_size.
    * �������� ��������� ������� zfs_get_filesize.
    * ������������ ����, ����������� �������� ����� ���������� ���� � ���������� ������������������.
    * ���������� ������ � ������� zfs_get_filesize, ��������� � ��������� ������������� ����������.
    * ��� ������������������� ���������� ������ ��������� ��������� ZFS_ERR_INVALID_HANDLE, ������������ � ������ �������� ��������� ���������.
    * ������ ��� ��������� ����� � void* �� UINT32.
    * �������� �������� clientId � ������� zfs_init_proxy().
    * ��������� ��������� CLIENT_KEY_SIZE � CLIENT_KEY_ID.
    * �������� ��� ������ ZFS_ERR_INVALID_CLIENT_ID.
    * � ��������� ������� �������� ���������� � ������ __stdcall.
*/

/**
 * ������ ����� ������:
 *    1Bit     7Bits      8Bits           16Bits
 *     .     ........   ........    ........  ........
 * [ErrFlag][ModuleID][FunctionID][--   ERROR CODE   --]
*/

#define ZFS_MODULE_SHIFT                        24
#define ZFS_FUNCTION_SHIFT                      16

#define ZFS_GETERROR(x)                         (((unsigned)x) & 0xFFFF)
#define ZFS_ERRFLAG                             0x80000000
#define ZFS_isERR(x)                            ((x) & ZFS_ERRFLAG)

// �������������� ���������.
#define ZFS_MODULE_PROXY                        0
#define ZFS_MODULE_IOMAN                        ((1 << ZFS_MODULE_SHIFT) | ZFS_ERRFLAG)
#define ZFS_MODULE_DIR                          ((2 << ZFS_MODULE_SHIFT) | ZFS_ERRFLAG)
#define ZFS_MODULE_FILE                         ((3 << ZFS_MODULE_SHIFT) | ZFS_ERRFLAG)
#define ZFS_MODULE                              ((4 << ZFS_MODULE_SHIFT) | ZFS_ERRFLAG)
#define ZFS_MODULE_FORMAT                       ((5 << ZFS_MODULE_SHIFT) | ZFS_ERRFLAG)

// �������������� �������.

// �������������� ������� IO ���������.
#define ZFS_UNMOUNTPARTITION                    ((1 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_IOMAN)
#define ZFS_FLUSHCACHE                          ((2 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_IOMAN)
#define ZFS_BLOCKREAD                           ((3 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_IOMAN)
#define ZFS_BLOCKWRITE                          ((4 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_IOMAN)
#define ZFS_USERDRIVER                          ((5 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_IOMAN)

// �������������� ������� ��� ������ � ����������.
#define ZFS_FINDNEXTINDIR                       ((1 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_DIR)
#define ZFS_FETCHENTRYWITHCONTEXT               ((2 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_DIR)
#define ZFS_PUSHENTRYWITHCONTEXT                ((3 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_DIR)
#define ZFS_GETDIRENTRY                         ((4 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_DIR)
#define ZFS_FINDFIRST                           ((5 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_DIR)
#define ZFS_FINDNEXT                            ((6 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_DIR)
#define ZFS_REWINDFIND                          ((7 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_DIR)
#define ZFS_FINDFREEDIRENT                      ((8 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_DIR)
#define ZFS_PUTENTRY                            ((9 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_DIR)
#define ZFS_CREATESHORTNAME                     ((10 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_DIR)
#define ZFS_CREATELFNS                          ((11 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_DIR)
#define ZFS_EXTENDDIRECTORY                     ((12 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_DIR)
#define ZFS_MKDIR                               ((13 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_DIR)

// �������������� ������� ��� ������ � �������.
#define ZFS_OPEN                                ((1 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_RMDIR                               ((2 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_MOVE                                ((3	<< ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_EXTENDFILE                          ((4 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_READ                                ((5 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_GETC                                ((6 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_GETLINE                             ((7 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_WRITE                               ((8 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_PUTC                                ((9 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_SEEK                                ((10 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_CHECKVALID                          ((11 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_CLOSE                               ((12 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_SETTIME                             ((13 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_BYTESLEFT                           ((14 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_SETENDOFFILE                        ((15 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_ISEOF                               ((16 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_TELL                                ((17 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_UNLINK                              ((18 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_GETTIME                             ((19 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_GETVERSION                          ((20 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)
#define ZFS_GETFILESIZE                         ((21 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE_FILE)

// �������������� ������� ��� ������ � �������� ��������.
#define ZFS_GETENTRY                            ((1 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE)
#define ZFS_CLEARCLUSTER                        ((2 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE)
#define ZFS_PUTZFSENTRY                         ((3 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE)
#define ZFS_FINDFREECLUSTER                     ((4 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE)
#define ZFS_COUNTFREECLUSTERS                   ((5 << ZFS_FUNCTION_SHIFT) | ZFS_MODULE)

//                                              0 +
#define ZFS_ERR_NULL_POINTER                    1   // ������� ��������� ��������� ZFS.
#define ZFS_ERR_NOT_ENOUGH_MEMORY               2   // �� ������� �������� ������.
#define ZFS_ERR_DEVICE_DRIVER_FAILED            3   // ������ �������� ����������.
#define ZFS_ERR_UNSUPPORTED_OPERATION           4   // �������� �� ��������������.
#define ZFS_ERR_DEVIO_FAILED                    5   // ������� DeviceIoControl ������ FALSE.
#define ZFS_ERR_INVALID_HANDLE                  6   // �������� ��������� �����.
#define ZFS_ERR_INVALID_CLIENT_ID               7   // �������� ������������� �������.

//                                              20 +
#define ZFS_ERR_ACTIVE_HANDLES                  20  // The partition cannot be unmounted until all active file handles are closed. (There may also be active handles on the cache).
#define ZFS_ERR_NOT_ENOUGH_FREE_SPACE	        21  // ��� ���������� �����.
#define ZFS_ERR_OUT_OF_BOUNDS_READ              22  // ������������� ���� �� ������ ��������� ����������� ��������� � �������� �������.
#define ZFS_ERR_OUT_OF_BOUNDS_WRITE             23  // ������������� ���� �� ������ ��������� ����������� ��������� � �������� �������.

//                                              30 +
#define ZFS_ERR_FILE_ALREADY_OPEN               30  // ���� ��� ����������.
#define ZFS_ERR_FILE_NOT_FOUND                  31  // ���� �� ������.
#define ZFS_ERR_FILE_OBJECT_IS_A_DIR            32  // �� �������� ������.
#define ZFS_ERR_FILE_IS_READ_ONLY               33  // ������� ������� ���� ��� ���������, ������� ����� ������� '������ ��� ������'.
#define ZFS_ERR_FILE_INVALID_PATH               34  // �� ���������� ���� � �����.
#define ZFS_ERR_FILE_NOT_OPENED_IN_WRITE_MODE   35  // ���� �� ������ � ������ ������.
#define ZFS_ERR_FILE_NOT_OPENED_IN_READ_MODE    36  // ���� �� ������ � ������ ������.
#define ZFS_ERR_FILE_DESTINATION_EXISTS         37  // ���������� ����� �� ���������� ��� �� �������� ������.
#define ZFS_ERR_FILE_DIR_NOT_FOUND              38  // �� ������ ��������� ���� � �����.
#define ZFS_ERR_FILE_BAD_HANDLE                 39  // �������� ��������� �����.

// Directory Error Codes                        50 +
#define ZFS_ERR_DIR_OBJECT_EXISTS               50  // ���� ��� ����� � ����� �� ������ ��� ���� � ��������� ����������.
#define ZFS_ERR_DIR_DIRECTORY_FULL              51  // ���������� ������������ (������ ������ ��������� ����� ��� ����� � ���� ����������).
#define ZFS_ERR_DIR_END_OF_DIR                  52  // ��������� ����� ����������.
#define ZFS_ERR_DIR_NOT_EMPTY                   53  // ������ ������� ���������� � ������� ������� ����� ��� �����.
#define ZFS_ERR_DIR_INVALID_PATH                54  // ��������� ���������� �� �������.
#define ZFS_ERR_DIR_EXTEND_FAILED               55  // �� ������� �����,����� ��������� ����������.
#define ZFS_ERR_DIR_NAME_TOO_LONG               56  // ����� ����� ��������� ���������� ������.
#define ZFS_ERR_DIR_NAME_BAD                    57  // � ����� ���������� ������������ �������.

//                                              70 +
#define ZFS_ERR_ZFS_NO_FREE_CLUSTERS            70  // �� ����� ��� ���������� �����.

typedef UINT32 zfs_handle_t;

#define ZFS_SEEK_SET 1  // � ������ �����.
#define ZFS_SEEK_CUR 2  // � ������� ������� � �����.
#define ZFS_SEEK_END 3  // � ����� �����.

#define ZFS_MAX_FILENAME 32

// ��������� ��� ������� zfs_open (�������� flags).
#define ZFS_MODE_READ       0x01    // ������ �� ������.
#define	ZFS_MODE_WRITE      0x02    // ������ �� ������.
#define ZFS_MODE_APPEND     0x04    // ����� ���������� ������ � ����� �����.
#define	ZFS_MODE_CREATE     0x08    // ���� ����� ������, ���� ��� �� ����������.
#define ZFS_MODE_TRUNCATE   0x10    // ���� ����� ������, ���� �� ����������.

// ��������� ��� ������� zfs_get_file_time (�������� aWhat).
#define ZFS_CREATED_TIME 1
#define ZFS_MODIFIED_TIME 2
#define ZFS_ACCESSED_TIME 4

#define CLIENT_KEY_SIZE 48
#define CLIENT_ID_SIZE 16

#pragma pack(push, 8)

typedef struct _zfs_buffer
{
    UINT32 sector;
    UINT32 lru;
    UINT16 numHandles;
    UINT16 persistance;
    UINT8 mode;
    char modified;
    char valid;
    UINT8* pBuffer;
} zfs_buffer_t, *pzfs_buffer_t;

typedef struct _zfs_fetch_context
{
    UINT32 ulChainLength;
    UINT32 ulDirCluster;
    UINT32 ulCurrentClusterLCN;
    UINT32 ulCurrentClusterNum;
    UINT32 ulCurrentEntry;
    zfs_buffer_t* pBuffer;
} zfs_fetch_context_t, *pzfs_fetch_context_t;

typedef struct _zfs_dir_entry
{
    UINT32 filesize;
    UINT32 objectCluster;
    UINT32 currentCluster;
    UINT32 addrCurrentCluster;
    UINT32 dirCluster;
    UINT16 currentItem;
    UINT32 createTime;   // Unix-����� ��������.
    UINT32 modifiedTime; // Unix-����� ���������.
    UINT32 accessedTime; // Unix-����� ���������� �������.
    char szWildCard[ZFS_MAX_FILENAME + 1];
    char fileName[ZFS_MAX_FILENAME + 1];
    UINT8 attrib;
    UINT8 special;
    zfs_fetch_context_t fetchContext;
} zfs_dir_entry_t, *pzfs_dir_entry_t;

#pragma pack(pop)


#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif // __cplusplus

/** �������������� ������ � �������� �������.

    ���������� 0 � ������ �������� �������������.
*/
EXTERN_C int __stdcall zfs_init_proxy(unsigned char key[CLIENT_KEY_SIZE], unsigned char clientId[CLIENT_ID_SIZE]);

/** ����������� ��� ������� � ��������� ������ � ZFS. */
EXTERN_C void __stdcall zfs_shutdown_proxy();

/** ������ ���������� �� ���������� ����.

    ���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_mkdir(char* dirName);

/** ������� ���������� �� ���������� ����.

    ���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_rmdir(char* dirName);

/** ���������� ������ �� �������� ��������� � �����.

    ���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_putc(zfs_handle_t handle, unsigned char ch);

/** ���������� ������ �� �������� ��������� � ����.

    pWritten - ���� ������, ������������ ���������� ���������� ������.
    ���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_write(zfs_handle_t handle, unsigned char* buffer, unsigned int size, unsigned int* pWritten);

/** ��������� ��������� �� ����� �����.

    ���������� 1 ���� ��������� ����� �����, ����� 0.

    � ������ ������������� ������ ���������� ������������� �����.
*/
EXTERN_C int __stdcall zfs_iseof(zfs_handle_t handle);

/** ���������� ������ �� �������� ��������� �� �����. 

    ���������� ��������� ���� ��� ������.
    ���� ��������� ����� �����, ���������� -1.
*/
EXTERN_C int __stdcall zfs_getc(zfs_handle_t handle);

/** ���������� ������ �� ����� (������ ������ ������������ �������� \n).

    ���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_getline(zfs_handle_t handle, char* buffer, int maxSize);

/** ������ ������ �� �������� ��������� �� �����.

    ���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_read(zfs_handle_t handle, unsigned char* buffer, unsigned int size, unsigned int* pReaded);

/** ���� ������ ���������� �� ���������� ����. 

    ���� ����������� ������� ��������� �������������� �������� (*, ?).

    ���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_findfirst(char* find_path, pzfs_dir_entry_t pDirent);

/** ���� ��������� ���������� �� ���� ���������� ��� ������ ������� zfs_findfirst.

    ���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_findnext(pzfs_dir_entry_t pDirent);

/** ��������� ����.

    ���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_move(char* srcPath, char* dstPath);

/** ������ ���� � ���������� ����������.

    ���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_open(zfs_handle_t* pHandle, char* fileName, unsigned char flags);

/** ��������� ������������ ��������� ���������.

    ���������� ��� ������ (0 - �������� ���������).
*/
EXTERN_C int __stdcall zfs_checkvalid(zfs_handle_t handle);

/** �������� ����� �������� ����.

    ���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_close(zfs_handle_t handle);

/** ���������� ������� ������� ��������� � �����.

    ���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_tell(zfs_handle_t handle, unsigned int* pFilePtr);

/** ���������� ��������� � �����.

    � �������� origin ���������� �������� ����� �� ZFS_SEEK_* ��������.

    ���������� ��� ������ (0 - �������).
    � ������ �������� ������������� �������� origin ������� ����� -3.
    � ������, ����� �������� ��������� ��������� �� �������, ������� ����� -2.
*/
EXTERN_C int __stdcall zfs_seek(zfs_handle_t handle, int filePtr, unsigned char origin);

/** ������� ���� �� ���������� ����.

    ���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_unlink(char* fileName);

/** �������� ����� ��������/���������/������� �����.

    ���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_get_time(char* fileName, unsigned int aWhat, unsigned int* uTime);

/** �������� ������ �������� ������.

    � ������� ����� ���������� ������ ������ (major = ret >> 4; minor = ret & 0x0F).
    � ������ ������, ���������� ��� ������.
*/
EXTERN_C int __stdcall zfs_get_version();

/** ���������� ������ �����.

	���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_get_filesize(zfs_handle_t handle, unsigned int* pFileSize);

/** ������� ���� �� ������� ������� � �����.

	���������� ��� ������ (0 - �������).
*/
EXTERN_C int __stdcall zfs_set_end_of_file(zfs_handle_t handle);

#endif // __ZFS_PROXY_H_
