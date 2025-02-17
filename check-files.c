/*
 * check-files.c
 *
 * Check that a set of files are up-to-date in the filesystem or
 * do not exist. Used to verify a patch target before doing a patch.
 *
 * Copyright (C) 2005 Linus Torvalds
 */
#include "cache.h"

static void check_file(const char *path)
{
	int fd = open(path, O_RDONLY);
	struct cache_entry *ce;
	struct stat st;
	int pos, changed;

	/* Nonexistent is fine */
	if (fd < 0) {
		if (errno != ENOENT)
			usage("%s: %s", path, strerror(errno));
		return;
	}

	/* Exists but is not in the cache is not fine */
	pos = cache_name_pos(path, strlen(path));
	if (pos < 0)
		usage("preparing to update existing file '%s' not in cache", path);
	ce = active_cache[pos];

	if (fstat(fd, &st) < 0)
		usage("fstat(%s): %s", path, strerror(errno));

	changed = cache_match_stat(ce, &st);
	if (changed)
		usage("preparing to update file '%s' not uptodate in cache", path);
}

int main(int argc, char **argv)
{
	int i;

	read_cache();
	for (i = 1; i < argc ; i++)
		check_file(argv[i]);
	return 0;
}
