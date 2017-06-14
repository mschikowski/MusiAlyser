#if !defined (MEDIAFILE_H)
#define MEDIAFILE_H
// (c) Reliable Software 2004

enum SourceType
{
	SRC_MIKE,
	SRC_WAVE,
	SRC_IMAGE,
	SRC_ANY,
	SRC_NONE
};

char const * GetFileFilter (SourceType type);

#endif
