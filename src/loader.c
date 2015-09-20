// Author: dibas
// Started: 18 Sep 2015
// Wii U homebrew poc to mount an sd card and read files stored on it
// Special thanks to Marionumber1, crediar and golden45 for helping me with any questions I have/had!

#include "loader.h"

void _start() 
{
    /* Load a good stack */
    asm(
        "lis %r1, 0x1ab5 ;"
        "ori %r1, %r1, 0xd138 ;"
    );

	/* Get a handle to coreinit.rpl */
	uint32_t coreinit_h;
	OSDynLoad_Acquire("coreinit.rpl", &coreinit_h);

	/* Memory allocation and FS functions */
	void* (*OSAllocFromSystem)(uint32_t size, int align);
	int (*FSInit)();
	int (*FSAddClient)(void *client, int unk1);
	int (*FSInitCmdBlock)(void *cmd);
	int (*FSOpenDir)(void *client, void *cmd, char *path, uint32_t *dir_handle, int unk1);
	int (*FSReadDir)(void *client, void *cmd, uint32_t dir_handle, void *buffer, int unk1);
	
	int (*FSGetMountSource)(void *client, void *cmd, int type, mount_source *source, int unk1);
	int (*FSMount)(void *client, void *cmd, mount_source *source, char *mountpath, uint32_t pathlength, int unk1);
	int (*FSOpenFile)(void *client, void *cmd, char *filepath, char *amode, uint32_t *file_handle, int unk1);
	int (*FSReadFile)(void *client, void *cmd, void *buffer, uint32_t size, uint32_t length, uint32_t file_handle, int unk1, int unk2);

	OSDynLoad_FindExport(coreinit_h, 0, "OSAllocFromSystem", &OSAllocFromSystem);
	OSDynLoad_FindExport(coreinit_h, 0, "FSInit", &FSInit);
	OSDynLoad_FindExport(coreinit_h, 0, "FSAddClient", &FSAddClient);
	OSDynLoad_FindExport(coreinit_h, 0, "FSInitCmdBlock", &FSInitCmdBlock);
	OSDynLoad_FindExport(coreinit_h, 0, "FSOpenDir", &FSOpenDir);
	OSDynLoad_FindExport(coreinit_h, 0, "FSReadDir", &FSReadDir);

	OSDynLoad_FindExport(coreinit_h, 0, "FSGetMountSource", &FSGetMountSource);
	OSDynLoad_FindExport(coreinit_h, 0, "FSMount", &FSMount);
	OSDynLoad_FindExport(coreinit_h, 0, "FSOpenFile", &FSOpenFile);
	OSDynLoad_FindExport(coreinit_h, 0, "FSReadFile", &FSReadFile);

	FSInit();

	/* Set up the client and command blocks */
	void *client = OSAllocFromSystem(0x1700, 0x20);
	void *cmd = OSAllocFromSystem(0xA80, 0x20);
	if (!(client && cmd)) OSFatal("Failed to allocate client and command block");

	FSAddClient(client, 0);
	FSInitCmdBlock(cmd);

	// todo: check permissions and throw exception if no mounting permissions available

	// OSLockMutex - Probably not. It's a single thread, nothing else can access this, Cross-F does this here
	mount_source m_source; // allocate mount source

	int ms_result = FSGetMountSource(client, cmd, 0, &m_source, 0); // type 0 = external device

	if(ms_result != 0) {
		char buf[256];
		__os_snprintf(buf, 256, "FSGetMountSource returned error code %d", ms_result);
		OSFatal(buf);
	}

	char mountPath[128]; // usually /vol/external01
	int m_result = FSMount(client, cmd, &m_source, mountPath, sizeof(mountPath), -1); 

	if(m_result != 0) {
		char buf[256];
		__os_snprintf(buf, 256, "FSMount returned error code %d", m_result);
		OSFatal(buf);
	}
	// OSUnlockMutex

	char defaultMountPath[] = "/vol/external01";

	if(!strcmp(mountPath, defaultMountPath)) {
		char buf[256];
		__os_snprintf(buf, 256, "FSMount returned nonstandard mount path: %s", mountPath);
		OSFatal(buf);
	}

	uint32_t file_handle;
	int open_result = FSOpenFile(client, cmd, "/vol/external01/SMASHD.txt", "r", &file_handle, 0);

	if(open_result != 0) {
		char buf[256];
		__os_snprintf(buf, 256, "FSOpenFile returned error code %d", open_result);
		OSFatal(buf);
	}

	uint32_t *file_buffer = OSAllocFromSystem(0x200, 0x20);
	int read_result = FSReadFile(client, cmd, file_buffer, 1, 25, file_handle, 0, -1); // todo: is size correct? one char one byte; read whole file, not just a few bytes

	if(read_result != 0) {
		char buf[256];
		__os_snprintf(buf, 256, "FSReadFile returned error code %d", read_result);
		OSFatal(buf);
	}

	char *message = (char*)&file_buffer[25];
	OSFatal(message);
}

int strcmp(char input[], char check[])	// http://stackoverflow.com/a/32266824 todo: add to string.c
{
    for (int i = 0;; i++)
    {
        if (input[i] == '\0' && check[i] == '\0')
        {
            break;
        }
        else if (input[i] == '\0' && check[i] != '\0')
        {
            return 1;
        }
        else if (input[i] != '\0' && check[i] == '\0')
        {
            return -1;
        }
        else if (input[i] > check[i])
        {
            return 1;
        }
        else if (input[i] < check[i])
        {
            return -1;
        }
        else
        {
            // characters are the same - continue and check next
        }
    }
    return 0;
}