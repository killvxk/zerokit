#ifndef __ZFS_WRAPPER_H_
#define __ZFS_WRAPPER_H_

#define ZFS_DEV 65500U
#define IOCTL_ZFS_WRAPPER_OPEN              CTL_CODE(ZFS_DEV, 0x901, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_READ              CTL_CODE(ZFS_DEV, 0x902, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_WRITE             CTL_CODE(ZFS_DEV, 0x903, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_CLOSE             CTL_CODE(ZFS_DEV, 0x904, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_UNLINK            CTL_CODE(ZFS_DEV, 0x906, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_MKDIR             CTL_CODE(ZFS_DEV, 0x907, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_TELL              CTL_CODE(ZFS_DEV, 0x908, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_ISEOF             CTL_CODE(ZFS_DEV, 0x909, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_RMDIR             CTL_CODE(ZFS_DEV, 0x90A, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_MOVE              CTL_CODE(ZFS_DEV, 0x90B, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_CHECKVALID        CTL_CODE(ZFS_DEV, 0x90C, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_GETC              CTL_CODE(ZFS_DEV, 0x90D, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_PUTC              CTL_CODE(ZFS_DEV, 0x90E, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_GETLINE           CTL_CODE(ZFS_DEV, 0x90F, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_SEEK              CTL_CODE(ZFS_DEV, 0x910, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_FINDFIRST         CTL_CODE(ZFS_DEV, 0x911, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_FINDNEXT          CTL_CODE(ZFS_DEV, 0x912, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_GET_TIME          CTL_CODE(ZFS_DEV, 0x913, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_GET_VERSION       CTL_CODE(ZFS_DEV, 0x914, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_GET_FILESIZE      CTL_CODE(ZFS_DEV, 0x915, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_ZFS_WRAPPER_SET_END_OF_FILE   CTL_CODE(ZFS_DEV, 0x916, METHOD_NEITHER, FILE_ANY_ACCESS)

enum {
    in_type,
    out_type,
    in_out_type
};

enum
{
    zfs_wrapper_open_id,
    zfs_wrapper_close_id,
    zfs_wrapper_read_id,
    zfs_wrapper_write_id,
    zfs_wrapper_seek_id,
    zfs_wrapper_unlink_id,
    zfs_wrapper_mkdir_id,
    zfs_wrapper_tell_id,
    zfs_wrapper_iseof_id,
    zfs_wrapper_rmdir_id,
    zfs_wrapper_move_id,
    zfs_wrapper_checkvalid_id,
    zfs_wrapper_getc_id,
    zfs_wrapper_putc_id,
    zfs_wrapper_getline_id,
    zfs_wrapper_findfirst_id,
    zfs_wrapper_findnext_id,
    zfs_wrapper_get_time_id,
    zfs_wrapper_get_version_id,
    zfs_wrapper_get_filesize_id,
    zfs_wrapper_setendoffile_id,
};

#define REQUEST_SIGNATURE 0x34798977

#ifndef CLIENT_ID_SIZE
#define CLIENT_ID_SIZE 16
#endif // CLIENT_ID_SIZE

#pragma pack(push, 1)

typedef struct _file_packet
{
    uint32_t signature; // ��������� ������� 0x34798977.
    uint32_t operation; // ID ��������.
    uint32_t handle;    // ��������� �����.
    int errCode;        // ��� ������.
    uint32_t param1;    // ��������� �������� ��� ����� ������.
    uint8_t clientId[CLIENT_ID_SIZE];   // ���������� ������������� �������.
    uint32_t dataSize;  // ������ ������.
    uint8_t data[1];    // ������.
} file_packet_t, *pfile_packet_t;

#define MAX_BUFFER_SIZE 0x1000;

typedef struct _file_data
{
    char fileName[MAX_PATH];    // ��� �����/����������.
    int filepoiner;             // ��� �������� ��������� ��������� ���������
    int bEof;                   // ��� �������� �������� ����� ����� ��� ��� �������� � �������� ����� - ����� ��������\�������� ��� ��� ������ ���� ������� �������� ����� (������������ ����� �������� ����� ��� �������)
    char origin;                // ��� �������� ��������� ��������� ��������� - ������������� ���������
    uint32_t fileSize;          // ������ ����� (������ ������������ ����� ��������\��������)
    uint32_t dataSize;          // ������ ������ ��� �������� ������ \ ������
    uint8_t data[1];            // ����� ������ \ ������  (�������� �� ����� ��������� MAX_BUFFER_SIZE �� ��������� �������� ����� ������� ������ (���� ����� �������������))
} file_data_t, *pfile_data_t;

#pragma pack()

#endif // __ZFS_WRAPPER_H_
