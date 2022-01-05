
//Test for bounds that are of a different type then the pointer.

/* zf: this yields 'invalid operands to binary -' in checks  */
unsigned int f(void *_p)
{
	unsigned char * COUNT(10) orig_p = TC(_p);
        unsigned char * SNT end_p = orig_p + 10;
 	unsigned int * BND(orig_p, end_p) p = (unsigned int * BND(orig_p, end_p))orig_p;
	return p[1];
}

int main(void) {
	unsigned int a[2] = {0,0};
	return f(a);
}


// An example of this from sendmail:
struct sm_file
{
  unsigned char   *BND(__this,max) f_p; /* current position in (some) buffer */
  char   *max;
};

void sm_io_getc(struct sm_file *fp)
{
    fp->f_p++;
}
