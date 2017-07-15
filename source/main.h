u8 regionCode;
u32 archive1;
u32 archive2;

FS_Archive ArchiveSD;
FS_Archive ArchiveHomeExt;
FS_Archive ArchiveThemeExt;

struct extArch
{
	Result ret;
	Handle handle;
	char* data;
	u64 size;
	u32 bytes;
};

typedef struct extArch extFile;


s8 openArchives();
s8 dumpExtdataSingle(extFile *file, const char *fileName, FS_Archive archive, FS_Archive newArchive);
s8 dumpExtdata();
