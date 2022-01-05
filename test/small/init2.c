
#include "harness.h"

// Test global initializers.  Make sure the checks are discharged statically.
TESTDEF baseline : success

char	*NTS (NT CompileOptions)[] =
{
	"ALLOW_255",
	"DNSMAP",
	"EGD",
        0
};
  
int main() {
  return 0;
}
