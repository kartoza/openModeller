Summary: Desktop interface for openModeller
Name: openModellerDesktop
Version: 1.1.0
Release: 1
License: GPL
Group: X11/Applications/Science
URL: http://openmodeller.sf.net
Source: %{name}-src-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Autoreqprov: on

%description
openModeller Desktop provides a flexible, user friendly, 
cross-platform environment where the entire process of 
conducting a species potential distribution modeling 
experiment can be carried out. openModeller Desktop 
includes facilities for reading species occurrence and 
environmental data, creating algorithm and layerset 
profiles, creating models, projecting models into
multiple environmental scenarios, testing models and
visualizing distribution maps.

%define prefix /usr
%define srcdirname %{name}-src-%{version}

%prep
%setup -q -n %{srcdirname}

%build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=%{prefix} -DPEDANTIC=OFF ../
make

%install
cd build
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc README.txt LICENSE.txt CHANGELOG.html CONTRIBUTORS.html
%{_bindir}/openModellerDesktop
%{_libdir}/libomg_core.so*
%{_libdir}/libomg_widgets.so*
%{_libdir}/openModellerDesktop
%{prefix}/include/openModellerDesktop/omgconfig.h
%{prefix}/share/openModellerDesktop/i18n

%changelog
* Thu May 28 2009 Renato De Giovanni <renato@cria.org.br> - 
- Initial build.

