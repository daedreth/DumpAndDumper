#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

int main(int argc, char **argv)
{
	gfxInitDefault();
	cfguInit();
	fsInit();
	consoleInit(GFX_TOP, NULL);

	s8 ret = openArchives();
	if(ret != 0)
	{	
		printf("\x1b[28;0HFailed to open extdata archives, woops.");
	}else{
		printf("\x1b[28;0HPress A to dump all theme extdata; Start to exit.");
	}

	while (aptMainLoop())
	{
		hidScanInput();
		u32 keyPress = hidKeysDown();
		
		if(keyPress & KEY_A)
		{
			s8 res = dumpExtdata();
			if(res == 0 && ret == 0) printf("\x1b[11;22HDone!");
		}

		if(keyPress & KEY_START) break;

		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}
	gfxExit();
 return 0;
}

s8 openArchives()
{
	Result retValue;

	FS_Path home;
	FS_Path theme;

	CFGU_SecureInfoGetRegion(&regionCode);
	switch(regionCode)
	{
		case 1:
			archive1 = 0x000002cd;
			archive2 = 0x0000008f;
			break;
		case 2:
			archive1 = 0x000002ce;
			archive2 = 0x00000098;
			break;
		case 3:
			archive1 = 0x000002cc;
			archive2 = 0x00000082;
			break;
		default:
			archive1 = 0x00;
			archive2 = 0x00;
	}

	retValue = FSUSER_OpenArchive(&ArchiveSD, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);

	u32 homeMenuPath[3] = {MEDIATYPE_SD, archive2, 0};
	home.type = PATH_BINARY;
	home.size = 0xC;
	home.data = homeMenuPath;
	retValue = FSUSER_OpenArchive(&ArchiveHomeExt, ARCHIVE_EXTDATA, home);	
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);

	u32 themePath[3] = {MEDIATYPE_SD, archive1, 0};
	theme.type = PATH_BINARY;
	theme.size = 0xC;
	theme.data = themePath;
	retValue = FSUSER_OpenArchive(&ArchiveThemeExt, ARCHIVE_EXTDATA, theme);	
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);

 return 0;
}

s8 dumpExtdataSingle(extFile *file, const char *fileName, FS_Archive archive, FS_Archive newArchive)
{
	file->ret = FSUSER_OpenFile(&file->handle, archive, fsMakePath(PATH_ASCII, fileName), FS_OPEN_READ, 0);
	if(R_FAILED(file->ret)) return R_SUMMARY(file->ret);
	file->ret = FSFILE_GetSize(file->handle, &file->size);
	if(R_FAILED(file->ret)) return R_SUMMARY(file->ret);
	file->data = malloc(file->size);
	file->ret = FSFILE_Read(file->handle, &file->bytes, 0, file->data, file->size);
	if(R_FAILED(file->ret)) return R_SUMMARY(file->ret);
	FSFILE_Close(file->handle);
	
	char *newPath = malloc(128);
	strcat(newPath, "/DumpAndDumper");
	strcat(newPath, fileName);
	file->ret = FSUSER_CreateFile(newArchive, fsMakePath(PATH_ASCII, newPath), 0, file->size);
	if(R_FAILED(file->ret)) return R_SUMMARY(file->ret);
	file->ret = FSUSER_OpenFile(&file->handle, newArchive, fsMakePath(PATH_ASCII, newPath), FS_OPEN_WRITE, 0);
	if(R_FAILED(file->ret)) return R_SUMMARY(file->ret);
	file->ret = FSFILE_Write(file->handle, &file->bytes, 0, file->data, file->size, FS_WRITE_FLUSH);
	if(R_FAILED(file->ret)) return R_SUMMARY(file->ret);
	FSFILE_Close(file->handle);
	
 return 0;
}

s8 dumpExtdata()
{

	extFile bodyCache, bgmCache, themeManage, saveData;

	Result retValue;

	FSUSER_DeleteDirectoryRecursively(ArchiveSD, fsMakePath(PATH_ASCII, "/DumpAndDumper"));
	retValue = FSUSER_CreateDirectory(ArchiveSD, fsMakePath(PATH_ASCII,"/DumpAndDumper") , FS_ATTRIBUTE_DIRECTORY);
	if(R_FAILED(retValue))
	{
		FSUSER_DeleteDirectoryRecursively(ArchiveSD, fsMakePath(PATH_ASCII, "/DumpAndDumper"));
		FSUSER_CreateDirectory(ArchiveSD, fsMakePath(PATH_ASCII,"/DumpAndDumper") , FS_ATTRIBUTE_DIRECTORY);
	}

	dumpExtdataSingle(&bodyCache, "/BodyCache.bin", ArchiveThemeExt, ArchiveSD);
	dumpExtdataSingle(&bgmCache, "/BgmCache.bin", ArchiveThemeExt, ArchiveSD);
	dumpExtdataSingle(&themeManage, "/ThemeManage.bin", ArchiveThemeExt, ArchiveSD);
	dumpExtdataSingle(&saveData, "/SaveData.dat", ArchiveHomeExt, ArchiveSD);
	

	retValue = FSUSER_CloseArchive(ArchiveSD);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	retValue = FSUSER_CloseArchive(ArchiveHomeExt);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	retValue = FSUSER_CloseArchive(ArchiveThemeExt);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);

 return 0;
}
