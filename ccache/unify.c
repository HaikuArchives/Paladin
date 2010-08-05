/*
 * Copyright (C) 2002 Andrew Tridgell
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

/*
 * C/C++ unifier
 *
 * The idea is that changes that don't affect the resulting C code should not
 * change the hash. This is achieved by folding white-space and other
 * non-semantic fluff in the input into a single unified format.
 *
 * This unifier was design to match the output of the unifier in compilercache,
 * which is flex based. The major difference is that this unifier is much
 * faster (about 2x) and more forgiving of syntactic errors. Continuing on
 * syntactic errors is important to cope with C/C++ extensions in the local
 * compiler (for example, inline assembly systems).
 */

#include "ccache.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

static const char *const s_tokens[] = {
	"...",	">>=",	"<<=",	"+=",	"-=",	"*=",	"/=",	"%=",	"&=",	"^=",
	"|=",	">>",	"<<",	"++",	"--",	"->",	"&&",	"||",	"<=",	">=",
	"==",	"!=",	";",	"{",	"<%",	"}",	"%>",	",",	":",	"=",
	"(",	")",	"[",	"<:",	"]",	":>",	".",	"&",	"!",	"~",
	"-",	"+",	"*",	"/",	"%",	"<",	">",	"^",	"|",	"?",
	0
};

#define C_ALPHA 1
#define C_SPACE 2
#define C_TOKEN 4
#define C_QUOTE 8
#define C_DIGIT 16
#define C_HEX   32
#define C_FLOAT 64
#define C_SIGN  128

static struct {
	unsigned char type;
	unsigned char num_toks;
	const char *toks[7];
} tokens[256];

/* build up the table used by the unifier */
static void build_table(void)
{
	unsigned char c;
	int i;
	static int done;

	if (done) return;
	done = 1;

	memset(tokens, 0, sizeof(tokens));
	for (c=0;c<128;c++) {
		if (isalpha(c) || c == '_') tokens[c].type |= C_ALPHA;
		if (isdigit(c)) tokens[c].type |= C_DIGIT;
		if (isspace(c)) tokens[c].type |= C_SPACE;
		if (isxdigit(c)) tokens[c].type |= C_HEX;
	}
	tokens['\''].type |= C_QUOTE;
	tokens['"'].type |= C_QUOTE;
	tokens['l'].type |= C_FLOAT;
	tokens['L'].type |= C_FLOAT;
	tokens['f'].type |= C_FLOAT;
	tokens['F'].type |= C_FLOAT;
	tokens['U'].type |= C_FLOAT;
	tokens['u'].type |= C_FLOAT;

	tokens['-'].type |= C_SIGN;
	tokens['+'].type |= C_SIGN;

	for (i=0;s_tokens[i];i++) {
		c = s_tokens[i][0];
		tokens[c].type |= C_TOKEN;
		tokens[c].toks[tokens[c].num_toks] = s_tokens[i];
		tokens[c].num_toks++;
	}
}

/* buffer up characters before hashing them */
static void pushchar(struct mdfour *hash, unsigned char c)
{
	static unsigned char buf[64];
	static size_t len;

	if (c == 0) {
		if (len > 0) {
			hash_buffer(hash, (char *)buf, len);
			len = 0;
		}
		hash_buffer(hash, NULL, 0);
		return;
	}

	buf[len++] = c;
	if (len == 64) {
		hash_buffer(hash, (char *)buf, len);
		len = 0;
	}
}

/* hash some C/C++ code after unifying */
static void unify(struct mdfour *hash, unsigned char *p, size_t size)
{
	size_t ofs;
	unsigned char q;
	int i;

	build_table();

	for (ofs=0; ofs<size;) {
		if (p[ofs] == '#') {
			if ((size-ofs) > 2 && p[ofs+1] == ' ' && isdigit(p[ofs+2])) {
				do {
					ofs++;
				} while (ofs < size && p[ofs] != '\n');
				ofs++;
			} else {
				do {
					pushchar(hash, p[ofs]);
					ofs++;
				} while (ofs < size && p[ofs] != '\n');
				pushchar(hash, '\n');
				ofs++;
			}
			continue;
		}

		if (tokens[p[ofs]].type & C_ALPHA) {
			do {
				pushchar(hash, p[ofs]);
				ofs++;
			} while (ofs < size &&
				 (tokens[p[ofs]].type & (C_ALPHA|C_DIGIT)));
			pushchar(hash, '\n');
			continue;
		}

		if (tokens[p[ofs]].type & C_DIGIT) {
			do {
				pushchar(hash, p[ofs]);
				ofs++;
			} while (ofs < size &&
				 ((tokens[p[ofs]].type & C_DIGIT) || p[ofs] == '.'));
			if (ofs < size && (p[ofs] == 'x' || p[ofs] == 'X')) {
				do {
					pushchar(hash, p[ofs]);
					ofs++;
				} while (ofs < size && (tokens[p[ofs]].type & C_HEX));
			}
			if (ofs < size && (p[ofs] == 'E' || p[ofs] == 'e')) {
				pushchar(hash, p[ofs]);
				ofs++;
				while (ofs < size &&
				       (tokens[p[ofs]].type & (C_DIGIT|C_SIGN))) {
					pushchar(hash, p[ofs]);
					ofs++;
				}
			}
			while (ofs < size && (tokens[p[ofs]].type & C_FLOAT)) {
				pushchar(hash, p[ofs]);
				ofs++;
			}
			pushchar(hash, '\n');
			continue;
		}

		if (tokens[p[ofs]].type & C_SPACE) {
			do {
				ofs++;
			} while (ofs < size && (tokens[p[ofs]].type & C_SPACE));
			continue;
		}

		if (tokens[p[ofs]].type & C_QUOTE) {
			q = p[ofs];
			pushchar(hash, p[ofs]);
			do {
				ofs++;
				while (ofs < size-1 && p[ofs] == '\\') {
					pushchar(hash, p[ofs]);
					pushchar(hash, p[ofs+1]);
					ofs+=2;
				}
				pushchar(hash, p[ofs]);
			} while (ofs < size && p[ofs] != q);
			pushchar(hash, '\n');
			ofs++;
			continue;
		}

		if (tokens[p[ofs]].type & C_TOKEN) {
			q = p[ofs];
			for (i=0;i<tokens[q].num_toks;i++) {
				unsigned char *s = (unsigned char *)tokens[q].toks[i];
				int len = strlen((char *)s);
				if (size >= ofs+len && memcmp(&p[ofs], s, len) == 0) {
					int j;
					for (j=0;s[j];j++) {
						pushchar(hash, s[j]);
						ofs++;
					}
					pushchar(hash, '\n');
					break;
				}
			}
			if (i < tokens[q].num_toks) {
				continue;
			}
		}

		pushchar(hash, p[ofs]);
		pushchar(hash, '\n');
		ofs++;
	}
	pushchar(hash, 0);
}


/* hash a file that consists of preprocessor output, but remove any line
   number information from the hash
*/
int unify_hash(struct mdfour *hash, const char *fname)
{
	int fd;
	struct stat st;
	char *map;

	fd = open(fname, O_RDONLY|O_BINARY);
	if (fd == -1 || fstat(fd, &st) != 0) {
		cc_log("Failed to open preprocessor output %s", fname);
		stats_update(STATS_PREPROCESSOR);
		return -1;
	}

	/* we use mmap() to make it easy to handle arbitrarily long
           lines in preprocessor output. I have seen lines of over
           100k in length, so this is well worth it */
	map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	if (map == (char *)-1) {
		cc_log("Failed to mmap %s", fname);
		return -1;
	}

	/* pass it through the unifier */
	unify(hash, (unsigned char *)map, st.st_size);

	munmap(map, st.st_size);

	return 0;
}
