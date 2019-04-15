#ifndef _ZFS_DEF_H_
#define _ZFS_DEF_H_

// MBR / PBR Offsets
#define ZFS_RESERVED_SECTORS    0x00E
#define ZFS_ROOT_ENTRY_COUNT    0x011
#define ZFS_TOTAL_SECTORS       0x020
#define ZFS_SECTORS_PER_ZFS     0x024
#define ZFS_ROOT_DIR_CLUSTER    0x02C

#define ZFS_DELETED             0xE5

/** ��������� �������� ��������
������������     ��������     ������     ��������

shortName        0x00         32         �������� ��� �����.
attr             0x20         1          ������� �����.
special          0x21         1          ���� ���������� ����������� ��������.
createTime       0x22         4          ����� �������� (Unix-time).
accessTime       0x26         4          ����� ���������� ������� (Unix-time).
modifTime        0x2A         4          ����� ���������� ��������� (Unix-time).
cluster          0x2E         4          ����� ������� �������� �����.
fileSize         0x32         4          ������ �����.
*/

// Directory Entry Offsets
#define ZFS_DIRENT_ATTRIB       0x020
#define ZFS_DIRENT_SPECIAL      0x021
#define ZFS_DIRENT_CREATE_TIME  0x022
#define ZFS_DIRENT_LASTACC_TIME 0x026
#define ZFS_DIRENT_LASTMOD_TIME 0x02A
#define ZFS_DIRENT_CLUSTER      0x02E
#define ZFS_DIRENT_FILESIZE     0x032
// #define ZFS_LFN_ORD             0x000
// #define ZFS_LFN_NAME_1          0x001
// #define	ZFS_LFN_CHECKSUM        0x00D
// #define ZFS_LFN_NAME_2          0x00E
// #define ZFS_LFN_NAME_3          0x01C

// Dirent Attributes
#define ZFS_ATTR_READONLY       0x01 // ���� ������ ��� ����������, �� ���� ����� ��������� ������ ��� ������.
#define ZFS_ATTR_VOLID          0x08
#define ZFS_ATTR_DIR            0x10
#define ZFS_ATTR_LFN            0x0F

#define ZFS_SPECIAL_SYSTEM      0x01 // ���� ������ ��� ����������, �� � �����/����� ����� ����� ������ (������) ������ ��������� ������.
// #define ZFS_CASE_OFFS           0x0C
// #define ZFS_CASE_ATTR_BASE      0x08
// #define ZFS_CASE_ATTR_EXT       0x10

#define ZFS_ENTRY_SIZE          0x40 // 64 �����.

#endif // _ZFS_DEF_H_
