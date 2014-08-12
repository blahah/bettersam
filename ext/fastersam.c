#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fastersam.h"

static char* alloc_and_copy(char *dst, char *src) {
  if (dst==NULL || strlen(dst)<strlen(src)) {
     if (dst!=NULL)
        free(dst);
     dst= malloc(sizeof (char)*(strlen(src)+1));
  }
  strcpy(dst, src);
  int len;
  len = strlen(dst);
  if (dst[len-1] == '\n') dst[len-1] = '\0';
  return dst;
}

static char* initialize(char *ptr) {
  if(ptr!=NULL){
    free(ptr);
    ptr = NULL;
  }
  return ptr;
}

int* initialize_int(int *ptr) {
  if(ptr!=NULL){
    free(ptr);
    ptr = NULL;
  }
  return ptr;
}

int check_header(char *header, char *firstline) {
  if (*header == *firstline)
    return 1;
  else {
    return 0;
  }
}

int int_from_str(char *str) {
  char *ptr;
  int ret = (int) strtol(str, &ptr, 10);
  return ret;
}

int parse_fields(char *str, char ***arr) {
  int count = 1;
  int token_len = 1;
  int i = 0;
  char c = '\t'; // TAB-delimited fields
  char *p;
  char *t;

  p = str;
  while (*p != '\0') {
    if (*p == c) {
      count++;
    }
    p++;
  }

  *arr = (char**) malloc(sizeof(char*) * count);
  if (*arr == NULL)
    exit(1);

  p = str;
  while (*p != '\0') {
    if (*p == c) {
      (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
      if ((*arr)[i] == NULL) {
        exit(1);
      }
      token_len = 0;
      i++;
    }
    p++;
    token_len++;
  }

  (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
  if ((*arr)[i] == NULL) {
    exit(1);
  }

  i = 0;
  p = str;
  t = ((*arr)[i]);
  while (*p != '\0') {
    if (*p != c && *p != '\0') {
      *t = *p;
      t++;
    } else {
      *t = '\0';
      i++;
      t = ((*arr)[i]);
    }
    p++;
  }

  return count;
}

void clear_record(SAMRecord *sam) {
  sam->qname = initialize(sam->qname);
  sam->flag  = initialize(sam->flag);
  sam->rname = initialize(sam->rname);
  sam->pos   = initialize(sam->pos);
  sam->mapq  = initialize(sam->mapq);
  sam->cigar = initialize(sam->cigar);
  sam->rnext = initialize(sam->rnext);
  sam->pnext = initialize(sam->pnext);
  sam->tlen  = initialize(sam->tlen);
  sam->seq   = initialize(sam->seq);
  sam->qual  = initialize(sam->qual);
}

void load_record(SAMRecord *sam, char **fields) {
  sam->qname = alloc_and_copy(sam->qname, fields[0]);
  sam->flag  = alloc_and_copy(sam->flag, fields[1]);
  sam->rname = alloc_and_copy(sam->rname, fields[2]);
  sam->pos   = alloc_and_copy(sam->pos, fields[3]);
  sam->mapq  = alloc_and_copy(sam->mapq, fields[4]);
  sam->cigar = alloc_and_copy(sam->cigar, fields[5]);
  sam->rnext = alloc_and_copy(sam->rnext, fields[6]);
  sam->pnext = alloc_and_copy(sam->pnext, fields[7]);
  sam->tlen  = alloc_and_copy(sam->tlen, fields[8]);
  sam->seq   = alloc_and_copy(sam->seq, fields[9]);
  sam->qual  = alloc_and_copy(sam->qual, fields[10]);
  // sam->tags  = fields[11];
}

int sam_iterator(SAMRecord *sam) {
  // intialise structure elements.
  char *header = "@"; // SAM header
  if (!sam->stream) {
    if(strcmp(sam->filename,"stdin") == 0) {
      sam->stream = stdin;
    }
    else {
      sam->stream = fopen(sam->filename,"r");
    }
  }
  if (!sam->line) {
    sam->line = malloc(sizeof (char)* _BSIZE);
  }

  // wipe out any data from previous iteration
  clear_record(sam);

  // load the next line
  if (fgets(sam->line, _BSIZE, sam->stream) == NULL) {
    fclose(sam->stream);
    return 0;
  }

  // parse the SAM record string into an array
  char **fields = NULL;
  int nfields = parse_fields(sam->line, &fields);
  if (nfields < 10) {
    return 1;
  }

  // load it into the struct
  load_record(sam, fields);

  return 1;
}

int main (int argc, char ** argv) {
  SAMRecord *s = malloc(sizeof(SAMRecord));
  char *path = "../test/data/basic.sam";
  s->filename = strdup(path);
  int ret = 1;
  while (ret == 1) {
    ret = sam_iterator(s);
    if (s->qname) {
      printf("%s\n",s->qname);
      printf("%i\n",atoi(s->tlen));
    }
  }
  return 0;
}


#undef _BSIZE