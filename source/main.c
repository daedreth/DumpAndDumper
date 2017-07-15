#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
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

s8 dumpExtdata()
{

	extFile bodyCache, bgmCache, themeManage, saveData;

	bodyCache.ret = FSUSER_OpenFile(&bodyCache.handle, ArchiveThemeExt, fsMakePath(PATH_ASCII, "/BodyCache.bin"), FS_OPEN_READ, 0);
	if(R_FAILED(bodyCache.ret)) return R_SUMMARY(bodyCache.ret);
	bodyCache.ret = FSFILE_GetSize(bodyCache.handle, &bodyCache.size);
	if(R_FAILED(bodyCache.ret)) return R_SUMMARY(bodyCache.ret);
	bodyCache.data = malloc(bodyCache.size);
	bodyCache.ret = FSFILE_Read(bodyCache.handle, &bodyCache.bytes, 0, bodyCache.data, bodyCache.size);
	if(R_FAILED(bodyCache.ret)) return R_SUMMARY(bodyCache.ret);
	FSFILE_Close(bodyCache.handle);
	
	bgmCache.ret = FSUSER_OpenFile(&bgmCache.handle, ArchiveThemeExt, fsMakePath(PATH_ASCII, "/BgmCache.bin"), FS_OPEN_READ, 0);
	if(R_FAILED(bgmCache.ret)) return R_SUMMARY(bgmCache.ret);
	bgmCache.ret = FSFILE_GetSize(bgmCache.handle, &bgmCache.size);
	if(R_FAILED(bgmCache.ret)) return R_SUMMARY(bgmCache.ret);
	bgmCache.data = malloc(bgmCache.size);
	bgmCache.ret = FSFILE_Read(bgmCache.handle, &bgmCache.bytes, 0, bgmCache.data, bgmCache.size);
	if(R_FAILED(bgmCache.ret)) return R_SUMMARY(bgmCache.ret);
	FSFILE_Close(bgmCache.handle);

	themeManage.ret = FSUSER_OpenFile(&themeManage.handle, ArchiveThemeExt, fsMakePath(PATH_ASCII, "/ThemeManage.bin"), FS_OPEN_READ, 0);
	if(R_FAILED(themeManage.ret)) return R_SUMMARY(themeManage.ret);
	themeManage.ret = FSFILE_GetSize(themeManage.handle, &themeManage.size);
	if(R_FAILED(themeManage.ret)) return R_SUMMARY(themeManage.ret);
	themeManage.data = malloc(themeManage.size);
	themeManage.ret = FSFILE_Read(themeManage.handle, &themeManage.bytes, 0, themeManage.data, themeManage.size);
	if(R_FAILED(themeManage.ret)) return R_SUMMARY(themeManage.ret);
	FSFILE_Close(themeManage.handle);

	saveData.ret = FSUSER_OpenFile(&saveData.handle, ArchiveHomeExt, fsMakePath(PATH_ASCII, "/SaveData.dat"), FS_OPEN_READ, 0);
	if(R_FAILED(saveData.ret)) return R_SUMMARY(saveData.ret);
	saveData.ret = FSFILE_GetSize(saveData.handle, &saveData.size);
	if(R_FAILED(saveData.ret)) return R_SUMMARY(saveData.ret);
	saveData.data = malloc(saveData.size);
	saveData.ret = FSFILE_Read(saveData.handle, &saveData.bytes, 0, saveData.data, saveData.size);
	if(R_FAILED(saveData.ret)) return R_SUMMARY(saveData.ret);
	FSFILE_Close(saveData.handle);

	Result retValue;
	FSUSER_DeleteDirectoryRecursively(ArchiveSD, fsMakePath(PATH_ASCII, "/DumpAndDumper"));
	retValue = FSUSER_CreateDirectory(ArchiveSD, fsMakePath(PATH_ASCII,"/DumpAndDumper") , FS_ATTRIBUTE_DIRECTORY);
	if(R_FAILED(retValue))
	{
		FSUSER_DeleteDirectoryRecursively(ArchiveSD, fsMakePath(PATH_ASCII, "/DumpAndDumper"));
		FSUSER_CreateDirectory(ArchiveSD, fsMakePath(PATH_ASCII,"/DumpAndDumper") , FS_ATTRIBUTE_DIRECTORY);
	}
	
	retValue = FSUSER_CreateFile(ArchiveSD, fsMakePath(PATH_ASCII, "/DumpAndDumper/BodyCache.bin"), 0, bodyCache.size);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	retValue = FSUSER_OpenFile(&bodyCache.handle, ArchiveSD, fsMakePath(PATH_ASCII, "/DumpAndDumper/BodyCache.bin"), FS_OPEN_WRITE, 0);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	retValue = FSFILE_Write(bodyCache.handle, &bodyCache.bytes, 0, bodyCache.data, bodyCache.size, FS_WRITE_FLUSH);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	FSFILE_Close(bodyCache.handle);
	
	retValue = FSUSER_CreateFile(ArchiveSD, fsMakePath(PATH_ASCII, "/DumpAndDumper/BgmCache.bin"), 0, bgmCache.size);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	retValue = FSUSER_OpenFile(&bgmCache.handle, ArchiveSD, fsMakePath(PATH_ASCII, "/DumpAndDumper/BgmCache.bin"), FS_OPEN_WRITE, 0);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	retValue = FSFILE_Write(bgmCache.handle, &bgmCache.bytes, 0, bgmCache.data, bgmCache.size, FS_WRITE_FLUSH);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	FSFILE_Close(bgmCache.handle);

	retValue = FSUSER_CreateFile(ArchiveSD, fsMakePath(PATH_ASCII, "/DumpAndDumper/ThemeManage.bin"), 0, themeManage.size);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	retValue = FSUSER_OpenFile(&themeManage.handle, ArchiveSD, fsMakePath(PATH_ASCII, "/DumpAndDumper/ThemeManage.bin"), FS_OPEN_WRITE, 0);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	retValue = FSFILE_Write(themeManage.handle, &themeManage.bytes, 0, themeManage.data, themeManage.size, FS_WRITE_FLUSH);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	FSFILE_Close(themeManage.handle);
	
	retValue = FSUSER_CreateFile(ArchiveSD, fsMakePath(PATH_ASCII, "/DumpAndDumper/SaveData.dat"), 0, saveData.size);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	retValue = FSUSER_OpenFile(&saveData.handle, ArchiveSD, fsMakePath(PATH_ASCII, "/DumpAndDumper/SaveData.dat"), FS_OPEN_WRITE, 0);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	retValue = FSFILE_Write(saveData.handle, &saveData.bytes, 0, saveData.data, saveData.size, FS_WRITE_FLUSH);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	FSFILE_Close(saveData.handle);

	retValue = FSUSER_CloseArchive(ArchiveSD);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	retValue = FSUSER_CloseArchive(ArchiveHomeExt);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);
	retValue = FSUSER_CloseArchive(ArchiveThemeExt);
	if(R_FAILED(retValue)) return R_SUMMARY(retValue);

 return 0;
}
