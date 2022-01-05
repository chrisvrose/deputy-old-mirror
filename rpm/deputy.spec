Summary: Deputy
Name: deputy
Version: 1.1
Release: 1
Source0: %{name}-%{version}.tar.gz
License: BSD
Group: Development/Languages
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}
AutoReq: no
AutoProv: no
Requires: perl >= 5.6.1, perl(FindBin), gcc >= 4
Provides: deputy = 1.1
%description
Deputy is an advanced C compiler that enforces memory and type safety in C
programs using annotations supplied by the programmer.
%prep
%setup -q
%build
./configure
make
%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
make install-man DESTDIR=$RPM_BUILD_ROOT
%clean
rm -rf $RPM_BUILD_ROOT
%files
%defattr(-,root,root)
/usr/local/bin/deputy
/usr/local/lib/deputy/bin/deputy
/usr/local/lib/deputy/bin/deputy.asm.exe
/usr/local/lib/deputy/bin/deputy.byte.exe
/usr/local/lib/deputy/lib/CilConfig.pm
/usr/local/lib/deputy/lib/Cilly.pm
/usr/local/lib/deputy/lib/Deputy.pm
/usr/local/lib/deputy/lib/KeptFile.pm
/usr/local/lib/deputy/lib/TempFile.pm
/usr/local/lib/deputy/lib/OutputFile.pm
/usr/local/lib/deputy/lib/deputy_libc.o
/usr/local/lib/deputy/include/libc_patch.i
/usr/local/lib/deputy/include/deputy/annots.h
/usr/local/lib/deputy/include/deputy/checks.h
%doc %attr(0444,root,root) /usr/local/man/man1/deputy.1.gz
