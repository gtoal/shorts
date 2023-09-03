#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <wchar.h>
#include <locale.h>
int main(int argc, char **argv) {
  FILE *left, *right;
  wint_t c;
  int i, maxwidth = 0, width;
  wchar_t leftline[1024], *rightline;
  char *ProgName;
  int leftdone, rightdone;
  int htmlmode = 0;
  
  auto int LEFT(void) {
    wint_t c;
    int i = 0;
    for (;;) {
      c = fgetwc(left);
      if (c == WEOF) break;
      if (c == L'\n') break;
      leftline[i++] = c;
    }
    return c==WEOF;
  }

  auto int RIGHT(void) {
    wint_t c;
    int i = 0;
    rightline[0] = L'\n';
    rightline[1] = '\0';
    for (;;) {
      c = fgetwc(right);
      if (c == WEOF) break;
      rightline[i++] = c; rightline[i] = '\0';
      if (c == L'\n') break;
    }
    return c==WEOF;
  }
  
  auto void htmlout(wchar_t *s) {
    while (*s) {
      wint_t c = *s++;
      if (c == L'<') {
        fwprintf(stdout, L"&lt;");
      } else if (c == L'>') {
        fwprintf(stdout, L"&gt;");
      } else if (c == L'&') {
        fwprintf(stdout, L"&amp;");
      } else if (c == L'‘') {
        fwprintf(stdout, L"&lsquo;");
      } else if (c == L'’') {
        fwprintf(stdout, L"&rsquo;");
      } else {
        fputwc(c, stdout);
      }
    }
  }
  /* char *locale = */ (void) setlocale(LC_ALL, "");  

  ProgName = argv[0];
  if (strrchr(ProgName, '/') != NULL) ProgName = strrchr(ProgName, '/')+1;
  if (argc > 1 && strcmp(argv[1], "--html")==0) {
    argc -= 1;
    argv += 1;
    htmlmode = 1;
  }

  if (argc != 3) {
    fprintf(stderr, "syntax: %s [--html] File1 File2\n", ProgName); exit(EXIT_SUCCESS);
  }
  
  left = fopen(argv[1], "r");
  right = fopen(argv[2], "r");
  if (!left) {
    fprintf(stderr, "%s: cannot open \"%s\" - %s\n", ProgName, argv[1], strerror(errno)); exit(EXIT_FAILURE);
  }
  if (!right) {
    fprintf(stderr, "%s: cannot open \"%s\" - %s\n", ProgName, argv[2], strerror(errno)); exit(EXIT_FAILURE);
  }
  for (;;) {
    width = 0;
    for (;;) {
      c = fgetwc(left);
      if (c == WEOF) break;
      if (c == L'\n') break;
      width++;
    }
    if (width > maxwidth) maxwidth = width;
    if (c == WEOF) break;
  }
  rewind(left);
  rightline = leftline+maxwidth+3;
  for (i = 0; i < 1024; i++) leftline[i] = L' ';
  leftline[maxwidth] = L' ';
  leftline[maxwidth+1] = L'|';
  leftline[maxwidth+2] = L' ';
  if (htmlmode) fwprintf(stdout, L"<html><head><meta charset=\"utf-8\"><title>%s vs %s</title></head><body><pre><font size=-0>", argv[1], argv[2]);
  for (;;) {
    leftdone = LEFT(); rightdone = RIGHT();
    if (leftdone && rightdone) break;
    if (htmlmode) htmlout(leftline); else fwprintf(stdout, L"%ls", leftline);
    for (i = 0; i < maxwidth; i++) leftline[i] = L' ';
  }
  if (htmlmode) fwprintf(stdout, L"</font></pre></body></html>\n");
  exit(EXIT_SUCCESS);
  return EXIT_FAILURE;
}
