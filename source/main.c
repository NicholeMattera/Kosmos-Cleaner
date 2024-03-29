/*
 * Kosmos Cleaner
 * Copyright (c) 2020 Nichole Mattera
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>
#include <sys/stat.h>
#include <unistd.h>

u32 __nx_applet_type = AppletType_None;

#define INNER_HEAP_SIZE 0x9000
size_t nx_inner_heap_size = INNER_HEAP_SIZE;
char   nx_inner_heap[INNER_HEAP_SIZE];

void __libnx_initheap(void) {
	void*  addr = nx_inner_heap;
	size_t size = nx_inner_heap_size;

	extern char* fake_heap_start;
	extern char* fake_heap_end;

	fake_heap_start = (char *) addr;
	fake_heap_end   = (char *) addr + size;
}

void __attribute__((weak)) __appInit(void) {
    Result rc;

    rc = smInitialize();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));

    rc = fsInitialize();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

    fsdevMountSdmc();
}

void __attribute__((weak)) userAppExit(void);

void __attribute__((weak)) __appExit(void) {
    fsdevUnmountAll();
    fsExit();
    smExit();
}

bool fileExists(const char * file) {
    struct stat buf;
    return (stat(file, &buf) == 0);
}

void deleteFile(char * path) {
    if (fileExists(path)) {
        remove(path); 
    }
}

void deleteFolder(char * path) {
    struct dirent * de;
    DIR * dir = opendir(path); 
  
    if (dir == NULL)
        return; 
  
    while ((de = readdir(dir)) != NULL) {
        if (de->d_type == DT_REG) {
            char * fullpath;
            fullpath = malloc(strlen(path) + strlen(de->d_name) + 2);
            strcpy(fullpath, path);
            strcat(fullpath, "/");
            strcat(fullpath, de->d_name);

            deleteFile(fullpath);

            free(fullpath);
        }
    }
  
    closedir(dir);
    rmdir(path);
}

int main(int argc, char * argv[]) {
    // Old Tesla
    deleteFolder("sdmc:/atmosphere/contents/010000000007E51A/flags");
    deleteFolder("sdmc:/atmosphere/contents/010000000007E51A");

    // Nichole Logo
    deleteFolder("sdmc:/atmosphere/exefs_patches/nichole_logo");

    // Delete itself
    deleteFolder("sdmc:/atmosphere/contents/010000000000DA7A/flags");
    deleteFolder("sdmc:/atmosphere/contents/010000000000DA7A");
    
    return 0;
}
