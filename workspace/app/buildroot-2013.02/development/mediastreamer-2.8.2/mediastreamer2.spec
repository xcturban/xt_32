# -*- rpm-spec -*-
# 
# mediastreamer2 -- A media streaming library for telephony applications
# 

%ifarch %ix86
%define		mediastreamer2_cpu	pentium4
%endif

Summary:	Audio/Video real-time streaming
Name:		mediastreamer
Version:	2.8.2
Release:	4
License:	GPL
Group:		Applications/Communications
URL:		http://www.mediastreamer2.com
Source0:	%{name}-2.8.2.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-buildroot
%ifarch %ix86
BuildArch:	i686
%endif

%description
Mediastreamer2 is a GPL licensed library to make audio and video
real-time streaming and processing. Written in pure C, it is based
upon the oRTP library.

%package        devel
Summary:        Headers, libraries and docs for the mediastreamer2 library
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description    devel
Mediastreamer2 is a GPL licensed library to make audio and video
real-time streaming and processing. Written in pure C, it is based
upon the ortp library.

This package contains header files and development libraries needed to
develop programs using the mediastreamer2 library.

%ifarch %ix86
%define	mediastreamer2_arch_cflags -malign-double -march=i686 -mtune=%{mediastreamer2_cpu}
%else
# Must be non-empty
%define mediastreamer2_arch_cflags -Wall
%endif
%define mediastreamer2_cflags %mediastreamer2_arch_cflags -Wall -g -pipe -pthread -O3 -fomit-frame-pointer -fno-schedule-insns -fschedule-insns2 -fstrict-aliasing

%define		video		%{?_without_video:0}%{!?_without_video:1}

%prep
%setup -q

%build
options="--enable-shared --enable-static --docdir=%{_docdir} "
%if !%{video}
options="$options --disable-video"
%endif

%configure $options
	
make -j$RPM_BUILD_NCPUS CFLAGS="%mediastreamer2_cflags" CXXFLAGS="%mediastreamer2_cflags"

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README
%{_bindir}/mediastream
%{_libdir}/*.so.*
%{_datadir}/images/nowebcamCIF.jpg
%{_datadir}/locale/??/LC_MESSAGES/mediastreamer.mo
%{_datadir}/locale/??_??/LC_MESSAGES/mediastreamer.mo

%files devel
%defattr(-,root,root,-)
%{_libdir}/*.la
%{_libdir}/*.a
%{_libdir}/*.so
%{_libdir}/pkgconfig/*.pc
%{_includedir}
%{_docdir}

%changelog
* Tue Oct 25 2005 Francois-Xavier Kowalski <fix@hp.com>
- Add to mediastreamer2 distribution with "make rpm" target
