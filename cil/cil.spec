Name: cil
Version: 1.3.5
Release: 1
License: BSD
URL: http://manju.cs.berkeley.edu/cil/
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
BuildRequires: gcc
BuildRequires: hevea
BuildRequires: ocaml >= 3.08
BuildRequires: perl >= 5.6.1

# No ELF executables or shared libraries
%define debug_package %{nil}


########################################################################
#
#  Package cil
#

Summary: OCaml library for C program analysis and transformation
Group: Development/Libraries
Requires: perl >= 5.6.1

%description
CIL (C Intermediate Language) is a high-level representation along
with a set of tools that permit easy analysis and source-to-source
transformation of C programs.

This package provides Perl modules which are useful for building
compiler wrappers.  A wrapper can use CIL to transform C code before
passing it along to the native C compiler.

%files
%defattr(-,root,root,-)
%doc LICENSE
%{_datadir}/%{name}


########################################################################
#
#  Package cil-devel
#

%package devel

Summary: OCaml library for C program analysis and transformation
Group: Development/Libraries
Requires: ocaml >= 3.04

%description devel
CIL (C Intermediate Language) is a high-level representation along
with a set of tools that permit easy analysis and source-to-source
transformation of C programs.

This package provides OCaml interfaces and an OCaml library which form
the CIL API.

%files devel
%defattr(-,root,root,-)
%doc LICENSE
%{_libdir}/%{name}


########################################################################
#
#  General scripts
#

%prep
%setup -q -n %{name}

%build
%configure
make cilversion machdep
make cillib NATIVECAML=
make cillib NATIVECAML=1

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT


%changelog
* Tue Aug  5 2003 Ben Liblit <liblit@cs.berkeley.edu> 
- Initial build.
