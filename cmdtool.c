// Copyright © 2022 Jonas Kümmerlin <jonas@kuemmerlin.eu>
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#include <stdio.h>
#include <dvdread/dvd_reader.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <sys/stat.h>

#ifdef _WIN32
#   include <windows.h>
#   include <tchar.h>
#   include "folderbrowsehelper.hpp"
#else
    typedef char TCHAR;
#   define _stprintf(buffer, size, ...) snprintf(buffer, size, __VA_ARGS__)
#   define _tmkdir(dir) mkdir(dir, 0755)
#   define _tfopen(file, mode) fopen(file, mode)
#   define _T(s) s
#   define _tcslen(s) strlen(s)
#   define _tcsdup(s) strdup(s)
#   define _tmain main
#   define _ftprintf(stream, ...) fprintf(stream, __VA_ARGS__)
#   define _tprintf(...) printf(__VA_ARGS__)
#   define _tcserror(n) strerror(n)
#endif /* WIN32 */


static TCHAR *
join_filenames(const TCHAR *dir, const TCHAR *file)
{
    size_t dirlen = _tcslen(dir);
    size_t fnlen = _tcslen(file);

    if (dirlen == 0) {
        return _tcsdup(file);
    }
    if (dir[dirlen-1] == '/'
#ifdef _WIN32
        || dir[dirlen-1] == '\\'
#endif
    ) {
        // dir contains trailing separator
        TCHAR *out = (TCHAR *)malloc((dirlen + fnlen + 1) * sizeof(TCHAR));
        memcpy(out, dir, dirlen * sizeof(TCHAR));
        memcpy(&out[dirlen], file, fnlen * sizeof(TCHAR));
        out[dirlen + fnlen] = 0;
        return out;
    } else {
        // need to insert slash
        TCHAR *out = (TCHAR *)malloc((dirlen + fnlen + 2) * sizeof(TCHAR));
        memcpy(out, dir, dirlen * sizeof(TCHAR));
#ifdef _WIN32
        out[dirlen] = '\\';
#else
        out[dirlen] = '/';
#endif
        memcpy(&out[dirlen + 1], file, fnlen * sizeof(TCHAR));
        out[dirlen + fnlen + 1] = 0;
        return out;
    }
}

static size_t
size_min(size_t a, size_t b)
{
    return a < b ? a : b;
}

static void
copy_bytes(dvd_file_t *f, const TCHAR *directory, const TCHAR *filename)
{
    TCHAR *name = join_filenames(directory, filename);
    FILE *o = _tfopen(name, _T("wb"));
    if (o) {
        size_t s = DVDFileSize(f) * 2048;
        size_t c = 0;
        while (c < s) {
            char buf[4096];
            ssize_t actual = DVDReadBytes(f, buf, size_min(s - c, sizeof(buf)));
            if (actual < 0) {
                fprintf(stderr, "DVDReadBytes error\n");
                break;
            } else {
                fwrite(buf, 1, actual, o);
                c += actual;
            }
        }

        fclose(o);
    } else {
        _ftprintf(stderr, _T("Couldn't open '%s': %s\n"), name, _tcserror(errno));
    }

    free(name);
}

static void
copy_blocks(dvd_file_t *f, const TCHAR *directory, const TCHAR *filename, size_t off, size_t num_blocks)
{
    TCHAR *name = join_filenames(directory, filename);
    FILE *o = _tfopen(name, _T("wb"));
    if (o) {
        size_t c = 0;
        while (c < num_blocks) {
            unsigned char buf[40960];
            size_t toread = size_min(num_blocks - c, sizeof(buf)/2048);
            ssize_t actual = DVDReadBlocks(f, (int)off + c, toread, buf);
            if (actual < 0) {
                fprintf(stderr, "DVDReadBlocks error\n");
                break;
            } else {
                fwrite(buf, 2048, actual, o);
                c += actual;
            }
        }

        fclose(o);
    } else {
        _ftprintf(stderr, _T("Couldn't open '%s': %s\n"), name, _tcserror(errno));
    }

    free(name);
}

static dvd_reader_t *
search_and_open_dvd(void)
{
#ifdef _WIN32
    for (char c = 'D'; c <= 'Z'; ++c) {
        char n[] = { c, ':', 0 };
        dvd_reader_t *r = DVDOpen(n);
        if (r) {
            _tprintf(_T("Found DVD in drive %c:\n"), (int)c);
            return r;
        }
    }
#else
    // XXX: nobody should have more than 10 optical drives
    const char *candidates[] = { "/dev/cdrom", "/dev/sr0", "/dev/sr1", "/dev/sr2",
        "/dev/sr3", "/dev/sr4", "/dev/sr5", "/dev/sr6", "/dev/sr7", "/dev/sr8", "/dev/sr9" };

    for (size_t i = 0; i < sizeof(candidates)/sizeof(candidates[0]); ++i) {
        dvd_reader_t *r = DVDOpen(candidates[i]);
        if (r) {
            printf("Found DVD in %s\n", candidates[i]);
            return r;
        }
    }
#endif

    return NULL;
}

int
_tmain(int argc, TCHAR **argv)
{
    if (argc != 2
#ifdef _WIN32
        && argc != 1
#endif
    ) {
        _ftprintf(stderr, _T("Usage: %s OUTDIR\n"), argc>0 ? argv[0] : _T("cmdtool"));
        return 1;
    }

    dvd_reader_t *r = search_and_open_dvd();
    if (!r) {
        _ftprintf(stderr, _T("ERROR: Could not open dvd drive\n"));
        return 1;
    }

    TCHAR *basedir = argv[1];
#ifdef _WIN32
    if (!basedir) {
        CoInitialize(NULL);
        FolderBrowseHelper_BrowseForFolder(GetConsoleWindow(), NULL, &basedir);

        if (!basedir) {
            _ftprintf(stderr, _T("ERROR: No folder passed as argument and no folder selected"));
            return 1;
        }
    }
#endif

    // create VIDEO_TS subdir
    TCHAR *outdir = join_filenames(basedir, _T("VIDEO_TS"));
    if (_tmkdir(outdir)) {
        _ftprintf(stderr, _T("Creating directory '%s': %s\n"), outdir, _tcserror(errno));
        return 1;
    }


    // copy all the things
    {
        dvd_file_t *f;

        f = DVDOpenFile(r, 0, DVD_READ_INFO_FILE);
        if (f) {
            _tprintf(_T("VIDEO_TS.IFO\n"));
            copy_bytes(f, outdir, _T("VIDEO_TS.IFO"));
            DVDCloseFile(f);
        }

        f = DVDOpenFile(r, 0, DVD_READ_INFO_BACKUP_FILE);
        if (f) {
            _tprintf(_T("VIDEO_TS.BUP\n"));
            copy_bytes(f, outdir, _T("VIDEO_TS.BUP"));
            DVDCloseFile(f);
        }

        f = DVDOpenFile(r, 0, DVD_READ_MENU_VOBS);
        if (f) {
            _tprintf(_T("VIDEO_TS.VOB\n"));
            copy_blocks(f, outdir, _T("VIDEO_TS.VOB"), 0, DVDFileSize(f));
            DVDCloseFile(f);
        }
    }

    for (int i = 1; i <= 99; ++i) {
        dvd_file_t *f;

        f = DVDOpenFile(r, i, DVD_READ_INFO_FILE);
        if (f) {
            TCHAR fn[13];
            _stprintf(fn, sizeof(fn)/sizeof(fn[0]), _T("VTS_%02d_0.IFO"), i);

            _tprintf(_T("%s\n"), fn);
            copy_bytes(f, outdir, fn);
            DVDCloseFile(f);
        }

        f = DVDOpenFile(r, i, DVD_READ_INFO_BACKUP_FILE);
        if (f) {
            TCHAR fn[13];
            _stprintf(fn, sizeof(fn)/sizeof(fn[0]), _T("VTS_%02d_0.BUP"), i);

            _tprintf(_T("%s\n"), fn);
            copy_bytes(f, outdir, fn);
            DVDCloseFile(f);
        }

        f = DVDOpenFile(r, i, DVD_READ_MENU_VOBS);
        if (f) {
            TCHAR fn[13];
            _stprintf(fn, sizeof(fn)/sizeof(fn[0]), _T("VTS_%02d_0.VOB"), i);

            _tprintf(_T("%s\n"), fn);
            copy_blocks(f, outdir, fn, 0, DVDFileSize(f));
            DVDCloseFile(f);
        }

        f = DVDOpenFile(r, i, DVD_READ_TITLE_VOBS);
        if (f) {
            size_t total_blocks = DVDFileSize(f);
            size_t part_blocks[9] = { 524288, 524288, 524288, 524288, 524288, 524288, 524288, 524288, 524288 };
            dvd_stat_t stat;
            if (DVDFileStat(r, i, DVD_READ_TITLE_VOBS, &stat) == 0) {
                for (int j = 0; j < stat.nr_parts; ++j) {
                    part_blocks[j] = stat.parts_size[j] / 2048;
                }
            }

            for (size_t off = 0, k = 1; off < total_blocks && k <= 9; off += part_blocks[k-1], ++k) {
                TCHAR fn[13];
                _stprintf(fn, sizeof(fn)/sizeof(fn[0]), _T("VTS_%02d_%1d.VOB"), i, (int)k);

                _tprintf(_T("%s\n"), fn);
                copy_blocks(f, outdir, fn, off, size_min(total_blocks - off, part_blocks[k-1]));
            }

            DVDCloseFile(f);
        }
    }


    DVDClose(r);
    return 0;
}
