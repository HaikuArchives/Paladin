Summary: A program to generate dependencies for C/C++ source files
Name: fastdep
Version: 0.16
Release: 1
Copyright: GPL
Group: Development/Tools
Source: http://www.irule.be/bvh/c++/fastdep/fastdep-0.16.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot

%description
Fastdep is a fast dependency generator for C/C++ files which generates
output that can be included in Makefiles.

%prep
%setup

%build
./configure
make

%install
mkdir -p $RPM_BUILD_ROOT/usr/bin
install -s -m 755 fastdep $RPM_BUILD_ROOT/usr/bin/fastdep

%clean
rm $RPM_BUILD_ROOT/usr/bin/fastdep

%files
%defattr(-,root,root)
%doc AUTHOR COPYING README TODO CHANGELOG "doc/book1.htm" "doc/c23.htm" "doc/c49.htm" "doc/c90.htm" "doc/fastdep.html" "doc/c100.htm" "doc/c34.htm" "doc/c63.htm" "doc/fastdep.pdf"

/usr/bin/fastdep
