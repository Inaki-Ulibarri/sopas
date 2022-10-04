/*******************************************/
/* TODO: - add reading puzzles from files  */
/*       - add getopt for cli options      */
/*******************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(foo, bar) (foo >= bar ? foo : bar)

/*
 * Check if string s is in array c, if it is,
 * return the index at which it is located, if not
 * return -1
 */
int
strIsIn(const char * s, const char * c)
{
        int sp = 0;
        int cp = 0;
        int start = 0;
        
        while (s[sp] && c[cp]) {
                if (s[sp] == c[cp]) {
                        ++sp;
			++cp;
		} else {
                        sp = 0;
			++cp;
			start = cp;
		}
	}
	
	return s[sp] == '\0' ? start : -1;
}

char **
allocMat(size_t x, size_t y)
{
	char ** m = calloc(y, sizeof(char**));
	for (size_t i = 0; i < y; ++i) {
		m[i] = calloc(x, sizeof(char*));
	}

	return m;
}

void
printMat(char ** m, size_t y)
{
	for (size_t i = 0; i < y && m[i][0] != ' '; ++i) {
		printf("%3zu: %s\n", i+1, m[i]);
	}		
}

void
freeMat(char ** m, size_t y)
{
	for (size_t i = 0; i < y; ++i) 
		free(m[i]);
	free(m);
}

/*
 * truncate a file at the first '\n'
 */
void
trNewLine(char * s)
{
        s[strcspn(s, "\n")] = 0;
}

/* 
 *convert all the elements of a string to upper
 */
void
strUpper(char * s)
{
        while(*s) {
                *s = toupper(*s);
                ++s;
        }
}

void
upperMat(char ** m, size_t y)
{
        for (size_t i = 0; i < y; ++i) {
                strUpper(m[i]);
        }
}

char **
userInputWordPuzzle(size_t * x, size_t * y)
{
        char ** mat  = NULL;
        char * ln    = NULL;
        size_t lnsz  = 0;
        size_t lnlen = 0;
        
        puts("Fill in the word puzzle, type in ':end' when you're done.");
	getline(&ln, &lnsz, stdin);
        trNewLine(ln);
        if (ln == NULL) {
                fprintf(stderr, "The line entered was not valid\n");
                exit(1);
        }
        lnlen = strlen(ln);
        mat = allocMat(lnlen, lnlen);
        strncpy(mat[0], ln, lnlen);
        for (size_t i = 1; i < lnlen; ++i) {
                fgets(ln, lnsz, stdin);
                trNewLine(ln);
                ln[lnlen] = 0;
                if (ln == NULL) {
                        fprintf(stderr, "The line entered was not valid\n");
                        exit(1);
                } else if (strncmp(ln, ":end", lnlen) == 0)  {
                        for (size_t j = i; j < lnlen; ++j)
                                memset(mat[j], ' ', lnlen);
                        break;
                }
                strncpy(mat[i], ln, lnlen);
        }
        
        free(ln);
        *x = lnlen;
        *y = lnlen;
        return mat;
}

char **
userInputLookFor(size_t * x, size_t * y)
{
        char ** m    = NULL;
        char *  ln   = NULL;
        size_t  lnsz = 0;
        int     wcnt = 0;

        do {
                printf("How many words do I look for? ");
                getline(&ln, &lnsz, stdin);
                wcnt = atoi(ln);
                if (wcnt <= 0)  
                        fprintf(stderr, "Invalid number of aguments\n");
        } while (wcnt <= 0);
        m = allocMat(lnsz, wcnt);
        for (int i = 0; i < wcnt; ++i)  {
                ln = fgets(ln, lnsz, stdin);
                trNewLine(ln);
                if (ln == NULL) {
                        fprintf(stderr, "The line entered was not valid\n");
                        exit(1);
                }
                strncpy(m[i], ln, lnsz);
        }
        
        free(ln);
        *x = lnsz;
        *y = wcnt;
        return m;
}

char **
copyMat(char ** m, size_t x, size_t y)
{
        char ** foo = allocMat(x, y);
        for (size_t i = 0; i < y; ++i) {
                strncpy(foo[i], m[i], x);
        }

        return foo;
}

void
revStr(const char * s, char * out)
{
        size_t l = strlen(s);
        for (size_t i = 0; i < l; ++i)
                out[i] = s[l-i-1];
        out[l] = '\0';
}

char **
solvePuzzle(char ** p, size_t px, size_t py,
            char ** l, size_t ly, size_t * found)
{
        char ** copy = copyMat(p, px, py);
        size_t bufsz = px+py;
        char * buf   = calloc(bufsz, sizeof(char));
        char * bak   = calloc(bufsz, sizeof(char));
        size_t f     = 0;
        int isfound  = 0;

        // horizontal lines
        for (size_t i = 0; i < ly; ++i) {
                isfound = 0;
                revStr(l[i], bak);
                for (size_t j = 0; j < py; ++j) {
                        int isin = strIsIn(l[i], p[j]);
                        int isbk = strIsIn(bak, p[j]);
                        if (isin != -1 || isbk != -1) {
                                isin = MAX(isin, isbk);
                                for (int k = 0; l[i][k]; ++k)
                                        copy[j][k+isin] = '%';
                                ++f;
                                isfound = 1;
                                break;
                        }
                }
                if (isfound) continue;
                // vertical lines
                for (size_t j = 0; j < px; ++j) {
                        // columns -> buf
                        for (size_t k = 0; k < py; ++k)
                                buf[k] = p[k][j];
                        buf[py] = '\0';
                        int isin = strIsIn(l[i], buf);
                        int isbk = strIsIn(bak, buf);
                        if (isin != -1 || isbk != -1) {
                                isin = MAX(isin, isbk);
                                for (size_t k = 0; l[i][k]; ++k)
					copy[k+isin][j] = '%';
                                ++f;
                                isfound = 1;
                                break;
                        }
                }
                if (isfound) continue;
                // oblique descending lines
                // Load one half of the puzzle and then the other half
                for (size_t offset = 0; offset < py && offset < px; ++offset) {
                        size_t end = 0;
                        // puzzle -> buf
                        for (size_t j = offset, m = 0; j < py; ++j, ++m) {
                                ++end;
                                buf[m] = p[j][m];
                        }
                        buf[end] = '\0';
                        int isin = strIsIn(l[i], buf);
                        int isbk = strIsIn(bak, buf);
                        if (isin != -1 || isbk != -1) {
                                isin = MAX(isin, isbk);
                                for (size_t j = offset+isin, m = isin, cnt = 0; l[i][cnt];
                                     ++j, ++m, ++cnt)
                                        copy[j][m] = '%';
                                ++f;
                                isfound = 1;
                                break;
                        }
                        // second half
                        end = 0;
                        for (size_t j = offset, m = 0 ; j < px; ++j, ++m) {
                                ++end;
                                buf[m] = p[m][j];
                        }
                        buf[end] = '\0';
                        isin = strIsIn(l[i], buf);
                        isbk = strIsIn(bak, buf);
                        if (isin != -1 || isbk != -1) {
                                isin = MAX(isin, isbk);
                                for (size_t j = offset+isin, m = isin, cnt = 0; l[i][cnt];
                                     ++j, ++m, ++cnt)
                                        copy[m][j] = '%';
                                f = offset == 0 ? f : f+1;
                                isfound = 1;
                                break;
                        }
                }
                if (isfound) continue;
                // oblique ascending line finding
                for (size_t offset = py-1; offset > 0; --offset) {
                        size_t end = 0;
                        for (long k = offset, m = 0; k >= 0; --k, ++m) {
                                buf[m] = p[k][m];
                                ++end;
                        }
                        buf[end] = '\0';
                        int isin = strIsIn(l[i], buf);
                        int isbk = strIsIn(bak, buf);
                        if (isin != -1 || isbk != -1) {
                                isin = MAX(isin, isbk);
                                for (long k = offset-isin, m = isin, cnt = 0; l[i][cnt];
                                     --k, ++m, ++cnt)
                                        copy[k][m] = '%';
                                ++f;
                                isfound = 1;
                                break;
                        }
                }
                if (isfound) continue;
                for (size_t offset = 0; offset < px; ++offset) {
                        size_t end = 0;
                        // line -> buf
                        for (long k = py-1, m = offset, cnt = 0; k >= 0;
                             --k, ++m, ++cnt) {
                                buf[cnt] = p[k][m];
                                ++end;
                        }
                        buf[end] = '\0';
                        int isin = strIsIn(l[i], buf);
                        int isbk = strIsIn(bak, buf);
                        if (isin != -1 || isbk != -1) {
                                isin = MAX(isin, isbk);
                                for (size_t k = py-1-isin, m = offset+isin, cnt = 0; l[i][cnt];
                                     --k, ++m, ++cnt)
                                        copy[k][m] = '%';
                                f = offset == 0 ? f : f+1;
                                isfound = 1;
                                break;
                        }
                }
        }

        free(bak);
        free(buf);
        *found = f;
        return copy;                    
}

int
main(void)
{
        char ** puzzle  = NULL;
        size_t  puzzlex = 0;
        size_t  puzzley = 0;
        char ** look    = NULL; 
        size_t  lookx   = 0;
        size_t  looky   = 0;
        char ** solve   = NULL;
        size_t  sfound  = 0;
                
        puzzle = userInputWordPuzzle(&puzzlex, &puzzley);
        look = userInputLookFor(&lookx, &looky);
	upperMat(puzzle, puzzley);
	upperMat(look, looky);        
        solve = solvePuzzle(puzzle, puzzlex, puzzley,
                            look, looky, &sfound);
	
        puts("----- OG Puzzle -----");
        printMat(puzzle, puzzley);
        puts("----- Look For -----");
        printMat(look, looky);
        puts("----- Solved Puzzle -----");
        printf("%zu / %zu words found\n", sfound, looky);
        printMat(solve, puzzley);

	freeMat(puzzle, puzzley);
        freeMat(look, looky);
        freeMat(solve, puzzley);
                 
	return EXIT_SUCCESS;
}
