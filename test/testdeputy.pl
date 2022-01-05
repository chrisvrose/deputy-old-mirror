require 5.000;
use strict;

use FindBin;
use lib "$FindBin::Bin/../cil/ocamlutil";

use RegTest;

$ENV{LANG} = 'C';

print "Test infrastructure for Deputy\n";

# Create our customized test harness
my $test = RegTest->new(AvailParams => { 'SUCCESS' => 0,
                                         'RUN' => 1, # Numeric
                                         'MEM' => 1 },
                        LogFile => "deputy.log",
                        CommandName => "testdeputy");

# am I on win32?
my $win32 = ($^O eq 'MSWin32' || $^O eq 'cygwin');
my $unix = !$win32;
my $linux = $^O eq 'linux';
my $solaris = $^O eq 'solaris';

my $make;
if ($solaris) {
    $make = "gmake";
} else {
    $make = "make";
}

# Start with a few tests that must be run first
$test->newTest(
    Name => "!inittests0",
    Dir => "..",
    Cmd => "$make",
    Group => ['ALWAYS']);

my %commonerrors = 
    (
# We are seeing an error from make. Try to classify it based on the stage
# in which we are
     "^make: \\*\\*\\*" => 
     sub { 
         if($_[1]->{ErrorCode} == 0) {
             my $err = defined $_[1]->{instage} > 0 ? $_[1]->{instage} : 1;
             $_[1]->{ErrorCode} = $err;
         }},
    
         # Collect some more parameters
         # Now error messages
    "^((Error|Bug|Unimplemented): .+)\$" 
                      => sub { if(! defined $_[1]->{ErrorMsg}) {
                                 $_[1]->{ErrorMsg} = $_[2];} },
    "^(.+ : error .+)\$" => sub { if(! defined $_[1]->{ErrorMsg}) {
                                     $_[1]->{ErrorMsg} = $_[2];} },
    "^(.+:\\d+: (Error|Unimplemented|Internal error):.+)\$" 
                     => sub { if(! defined $_[1]->{ErrorMsg}) {
                                       $_[1]->{ErrorMsg} = $_[2];} },
    "^(.+: fatal error.+)\$" => sub { if(! defined $_[1]->{ErrorMsg}) {
                                          $_[1]->{ErrorMsg} = $_[2];} },
    "^(.+: Assertion failed in.+)\$" => sub { if(! defined $_[1]->{ErrorMsg}) {
                                          $_[1]->{ErrorMsg} = $_[2];} },
    "^(stackdump: Dumping stack trace)" => 
                   sub { if(! defined $_[1]->{ErrorMsg}) {
                         $_[1]->{ErrorMsg} = $_[2];} },
    "^(Cannot find .* in output of .*) at " => 
                   sub { if(! defined $_[1]->{ErrorMsg}) {
                         $_[1]->{ErrorMsg} = $_[2];} },

# Running time measurements
    "^user\\s+(\\d+)m([\\d.]+)s"
              => sub { $_[1]->{RUN} = 60 * $_[2] + $_[3]; },

    );


# Deputy tests
# First argument is a command (one word test name followed by arguments for
# make)
sub addDeputyTest {
    my($command, %extrafields) = @_;

    my ($name, $extraargs) = 
        ($command =~ /^(\S+) ?(.*)$/);     # name is first word

    # Make a local copy of the hash 
    my %patterns = %commonerrors;
    my $tst =
        $test->newTest(Name => $name,
                       Dir => ".",
                       Cmd => "$make " . $name . " " . $extraargs,
                       Group => [ ],
                       Patterns => \%patterns);


    # Add the extra fields
    my $key;
    foreach $key (keys %extrafields) {
        $tst->{$key} = $extrafields{$key};
    }
    return $tst;
}

addDeputyTest("small/abstract1");

addDeputyTest("small/addrof1");
addDeputyTest("small/addrof2");
addDeputyTest("small/addrof3");
addDeputyTest("small/addrof4");
addDeputyTest("small/addrof5");
addDeputyTest("small/addrof6");
addDeputyTest("small/addrof7");

addDeputyTest("small/align1");

addDeputyTest("small/alloc1");
addDeputyTest("small/alloc2");
addDeputyTest("small/alloc3");
addDeputyTest("small/alloc4");
addDeputyTest("small/alloc5");
addDeputyTest("small/alloc6");
addDeputyTest("small/alloc7");
addDeputyTest("small/alloc8");
addDeputyTest("small/alloc9");
addDeputyTest("small/alloc10");

addDeputyTest("small/array1");
addDeputyTest("small/array2");
addDeputyTest("small/array3");
addDeputyTest("small/array4");
addDeputyTest("small/array5");

addDeputyTest("small/auto1");
addDeputyTest("small/auto2");
addDeputyTest("small/auto3");
addDeputyTest("small/auto4",
              Comm => "Better error messages for illegal auto use. Bug 41.");
addDeputyTest("small/auto5");
addDeputyTest("small/auto6");
addDeputyTest("small/auto7");
addDeputyTest("small/auto8");

addDeputyTest("small/bound1");

addDeputyTest("small/builtin1");

addDeputyTest("small/call1");
addDeputyTest("small/call2");
addDeputyTest("small/call3");
addDeputyTest("small/call4");
addDeputyTest("small/call5");
addDeputyTest("small/call6");
addDeputyTest("small/call7");
addDeputyTest("small/call8",
              Comm => "Better checking for args in ret type. Bug 46.");

addDeputyTest("small/cast1");
addDeputyTest("small/cast2");
addDeputyTest("small/cast3");
addDeputyTest("small/cast4");
addDeputyTest("small/cast5");
addDeputyTest("small/cast6");
addDeputyTest("small/cast7");
addDeputyTest("small/cast8");
addDeputyTest("small/cast9");
addDeputyTest("small/cast10");
addDeputyTest("small/cast11");
addDeputyTest("small/cast12");
addDeputyTest("small/cast13");
addDeputyTest("small/cast14");
addDeputyTest("small/cast15", 
              Comm => "Physical sutyping. Bug 5");
addDeputyTest("small/cast16", 
              Comm => "Rmunused removes meta variables. Low priority");
addDeputyTest("small/cast17");
addDeputyTest("small/cast18");
addDeputyTest("small/cast20");

addDeputyTest("small/deref1");
addDeputyTest("small/deref2");
addDeputyTest("small/deref3");

addDeputyTest("small/enum1");

addDeputyTest("small/extern1",
              Comm => "Handle nullterm on extern arrays");
addDeputyTest("small/extern2");
addDeputyTest("small/extern3");

addDeputyTest("small/extinline1");

addDeputyTest("small/field1");
addDeputyTest("small/field2");
addDeputyTest("small/field3");
addDeputyTest("small/field4");
addDeputyTest("small/field5");

addDeputyTest("small/func1");
addDeputyTest("small/func2");
addDeputyTest("small/func3",
        Comm => "Dependencies of function arguments on non-const globals");
addDeputyTest("small/func4");
addDeputyTest("small/func5");
addDeputyTest("small/func6");
addDeputyTest("small/func7");

addDeputyTest("small/global1");
addDeputyTest("small/global2");
addDeputyTest("small/global3");
addDeputyTest("small/global4");
addDeputyTest("small/global5");
addDeputyTest("small/global6");
addDeputyTest("small/global7");
addDeputyTest("small/global8");
addDeputyTest("small/global9");

addDeputyTest("small/incr1");

addDeputyTest("small/infer1");
addDeputyTest("small/infer2");
addDeputyTest("small/infer3");
addDeputyTest("small/infer4");
addDeputyTest("small/infer5");
addDeputyTest("small/infer6");
addDeputyTest("small/infer7");
addDeputyTest("small/infer8");
addDeputyTest("small/infer9");
addDeputyTest("small/infer10");
addDeputyTest("small/infer11");
addDeputyTest("small/infer12");
addDeputyTest("small/infer13");
addDeputyTest("small/infer14");
addDeputyTest("small/infer15");
addDeputyTest("small/infer16");
addDeputyTest("small/infer17");
addDeputyTest("small/infer18");
addDeputyTest("small/infer19");

addDeputyTest("small/init1");
addDeputyTest("small/init2");

addDeputyTest("small/live1");
addDeputyTest("small/live2");
addDeputyTest("small/live3");

addDeputyTest("small/local1");

addDeputyTest("small/memcmp1");
addDeputyTest("small/memcmp2");
addDeputyTest("small/memcpy1");
addDeputyTest("small/memcpy2");
addDeputyTest("small/memset1");

addDeputyTest("small/nonnull1");
addDeputyTest("small/nonnull2");
addDeputyTest("small/nonnull3");
addDeputyTest("small/nonnull4");

addDeputyTest("small/nullterm1");
addDeputyTest("small/nullterm2");
addDeputyTest("small/nullterm3");
addDeputyTest("small/nullterm4");
addDeputyTest("small/nullterm5");
addDeputyTest("small/nullterm6");

addDeputyTest("small/offset1");
addDeputyTest("small/offset2");
addDeputyTest("small/offset3");

addDeputyTest("small/openarray1");
addDeputyTest("small/openarray2",
              Comm => "Nullterm open arrays");
addDeputyTest("small/openarray3");

addDeputyTest("small/opt1");
addDeputyTest("small/opt2");
addDeputyTest("small/opt3");
addDeputyTest("small/opt4");
addDeputyTest("small/opt5");
addDeputyTest("small/opt6");
addDeputyTest("small/opt7 EXTRAARGS=-fwritable-strings", 
              Comm => "don't copy string literals around during optimization.");
addDeputyTest("small/opt8",
              Comm => "Need better optimization");
addDeputyTest("small/opt9");
addDeputyTest("small/opt10");
addDeputyTest("small/opt11");
addDeputyTest("small/opt12");
addDeputyTest("small/opt13");
addDeputyTest("small/opt14");
addDeputyTest("small/opt15");
addDeputyTest("small/opt16");

addDeputyTest("small/overflow1");

addDeputyTest("small/poly1");
addDeputyTest("small/poly2",
              Comm => "Need to handle temporaries with poly type.");
addDeputyTest("small/poly3");
addDeputyTest("small/poly4");
addDeputyTest("small/poly5");
addDeputyTest("small/poly6");

addDeputyTest("small/ptrarith1");

addDeputyTest("small/return1");

addDeputyTest("small/sentinel1");
addDeputyTest("small/sentinel2");

addDeputyTest("small/size1");
addDeputyTest("small/size2");
addDeputyTest("small/size3");
addDeputyTest("small/size4");

addDeputyTest("small/sizeof1");
addDeputyTest("small/sizeof2");
addDeputyTest("small/sizeof3");

addDeputyTest("small/startof1");
addDeputyTest("small/startof2");

addDeputyTest("small/string1");
addDeputyTest("small/string2");
addDeputyTest("small/string3");
addDeputyTest("small/string4");
addDeputyTest("small/string5");
addDeputyTest("small/string6");
addDeputyTest("small/string7");
addDeputyTest("small/string8");
addDeputyTest("small/string9");
addDeputyTest("small/string10");
addDeputyTest("small/string12");
addDeputyTest("small/string13");
addDeputyTest("small/string14");
addDeputyTest("small/string15");
addDeputyTest("small/string16");
addDeputyTest("small/string18");
addDeputyTest("small/string19");
addDeputyTest("small/string20");
addDeputyTest("small/string21",
              Comm => "while (*s++) on nullterm pointer");

addDeputyTest("small/struct1");

addDeputyTest("small/trusted1");
addDeputyTest("small/trusted2");
addDeputyTest("small/trusted3");
addDeputyTest("small/trusted4");
addDeputyTest("small/trusted5");
addDeputyTest("small/trusted6");
addDeputyTest("small/trusted7");
addDeputyTest("small/trusted8");
addDeputyTest("small/trusted9",
              Comm => "Trusted open arrays");
addDeputyTest("small/trusted10");
addDeputyTest("small/trusted11");

addDeputyTest("small/typedef1");
addDeputyTest("small/typedef2",
              Comm => "Arrays changed to pointers in function arguments. Need count. Low priority");

addDeputyTest("small/types1");
addDeputyTest("small/types2");
addDeputyTest("small/types3");
addDeputyTest("small/types4");
addDeputyTest("small/types5");
addDeputyTest("small/types6");
addDeputyTest("small/types7");
addDeputyTest("small/types8");
addDeputyTest("small/types9");

addDeputyTest("small/union1");
addDeputyTest("small/union2");
addDeputyTest("small/union3");
addDeputyTest("small/union4");
addDeputyTest("small/union5");
addDeputyTest("small/union6");

addDeputyTest("small/upcast1");
addDeputyTest("small/upcast2");

addDeputyTest("small/var1");
addDeputyTest("small/var2");
addDeputyTest("small/var3");
addDeputyTest("small/var4");
addDeputyTest("small/var5");

addDeputyTest("small/vararg1");

addDeputyTest("small/voidstar1");
addDeputyTest("small/voidstar2");

addDeputyTest("small/voidstar4");

addDeputyTest("small/volatile1");

# libc tests

addDeputyTest("libc/crypt1 EXTRAARGS=-lcrypt");
addDeputyTest("libc/ctype1");
addDeputyTest("libc/fwrite1");
addDeputyTest("libc/getpwnam1");
addDeputyTest("libc/glob1");
addDeputyTest("libc/hostent1");
 # gethostent_r doesn't exist on Cygwin:
addDeputyTest("libc/hostent2") if $linux;
addDeputyTest("libc/malloc1");
addDeputyTest("libc/malloc2");
addDeputyTest("libc/memset1");
addDeputyTest("libc/popen1");
addDeputyTest("libc/printf1");
addDeputyTest("libc/printf2");
addDeputyTest("libc/readv1");
addDeputyTest("libc/servent1");
addDeputyTest("libc/servent2") if $linux;
addDeputyTest("libc/stat1");
addDeputyTest("libc/strchr1");
addDeputyTest("libc/strcpy");
addDeputyTest("libc/strerror1");
addDeputyTest("libc/string1");
addDeputyTest("libc/string2");
addDeputyTest("libc/string3");
addDeputyTest("libc/string4");
addDeputyTest("libc/string5",
              Comm => "old strings-david. Not yet ported?");

addDeputyTest("libc/string6");
addDeputyTest("libc/string7",
              Comm => "old strings-zach. not yet ported?");

 #Manju doesn't support strlcpy:
addDeputyTest("libc/strlcpy") if $win32;
addDeputyTest("libc/strncpy1");
addDeputyTest("libc/strpbrk1");
addDeputyTest("libc/strspn");
addDeputyTest("libc/strtok1");
addDeputyTest("libc/vararg1");
addDeputyTest("libc/writev1");

#addDeputyTest("libc/getaddrinfo1");
#addDeputyTest("libc/sockaddr1");
#addDeputyTest("libc/socket1");

# See if we need to include any other tests
if(defined $ENV{DEPUTYEXTRATESTS}) {
    require $ENV{DEPUTYEXTRATESTS};
}

######################################################################

# Now run the tests.
$test->doit();

# print Dumper($test);

exit(0);

1;
