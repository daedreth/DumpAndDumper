u8 regionCode;
u32 archive1;
u32 archive2;

FS_Archive ArchiveSD;
FS_Archive ArchiveHomeExt;
FS_Archive ArchiveThemeExt;

typedef struct
{
	Result ret;
	Handle handle;
	char* data;
	u64 size;
	u32 bytes;
}extFile;


s8 openArchives();
s8 dumpExtdata();
