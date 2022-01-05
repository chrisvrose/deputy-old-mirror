// getpwnam.c
// test getpwnam wrapper

#include <stdlib.h>       // exit, free
#include <pwd.h>          // getpwnam
#include <grp.h>          // getgrnam
#include <sys/types.h>    // uid_t?
#include <stdio.h>        // printf
#include <string.h>       // printf
#include <unistd.h>       // getuid

#define E(n) { printf("Error %d\n", n); exit(n); }
#ifndef DEPUTY
  #define NTS
#endif

TESTDEF : success

int main()
{
  int zero = 0;
  struct passwd *pw = getpwuid(getuid());
  struct group  *gr = getgrgid(getgid());
  char* username = strdup(pw->pw_name);
  pw = getpwnam(username);

  printf("name: %s\n", pw->pw_name);
  printf("uid: %d\n", pw->pw_uid);
  printf("gid: %d\n", pw->pw_gid);
  printf("passwd: %p\n", (unsigned long)pw->pw_passwd);
  printf("passwd: %s\n", pw->pw_passwd);


  
  printf("gr name: %s\n", gr->gr_name);
  printf("gid: %d\n",     gr->gr_gid);
  printf("passwd: %p\n",  (unsigned long)gr->gr_passwd);
  printf("gr_mem: %p\n",  (unsigned long)gr->gr_mem);
 
  if(strcmp(pw->pw_name, username)) E(1);
  if(pw->pw_uid != getuid()) E(2);
  if(pw->pw_gid != getgid()) E(3);
  
  free(username);
                       
  // make us infer non-simple representations
  pw->pw_name += zero;

IFTEST : error = will always fail
  // We cannot decrement an NT pointer
  pw->pw_passwd++;
  pw->pw_passwd--;      
ENDIF
    
  // Do the same for the group structure
  gr->gr_name += zero;
  gr->gr_mem  += zero; // Make it FSEQ
  if(gr->gr_mem && *gr->gr_mem) {
    char * foo = *gr->gr_mem - zero; // Make that SEQ 
  }


  // List the members in my group
  {
    char * NTS * p = gr->gr_mem;
    while(*p) {
      // Check the group for this user
      struct passwd *pw = getpwnam(*p);
      printf("  group member: %s\n", *p);
      printf("     name: %s\n", pw->pw_name);
      printf("     uid: %d\n", pw->pw_uid);
      printf("     gid: %d\n", pw->pw_gid);
      printf("     passwd: %p\n", (unsigned long)pw->pw_passwd);
      printf("     passwd: %s\n", pw->pw_passwd);
       


      if(strcmp(pw->pw_name, *p)) E(11);
      // if(pw->pw_gid != gr->gr_gid) E(12);
      // Try next user
      p ++;
    }
  }
  printf("Success\n"); exit(0);
}
