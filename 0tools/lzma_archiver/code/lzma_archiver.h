#ifndef __LZMA_ARCH_H_
#define __LZMA_ARCH_H_

#pragma pack(push, 1)

typedef struct _lzma_arch_header
{
    unsigned int signature; // 0x79977997
    unsigned int numFiles; // ���������� ������ � ������.
    unsigned int totalSize; // ������ ����� ������, ������� ���������.
} lzma_arch_header_t, *plzma_arch_header_t;

#pragma pack(pop)

/** ������ ������ �������� ������:

 - ��� �������� ������ �� ������������ ������� '\0'.
 - 4 ����� ����� ��������.
 - 4 ����� � �������.
 - ������.
*/

#define LAF_ENTRY_COMPRESSED 0x00000001

#endif // __LZMA_ARCH_H_
