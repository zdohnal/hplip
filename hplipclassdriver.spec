Name: hplipclassdriver
Version: 3.18.4
Release: 0
License: (c) 2004-2009 Copyright HP Development Company, LP
Packager: HPLIP
Vendor: HP
Summary: HPLIP
Group: Applications
%post
ldconfig
%description 
The HP Linux Imaging and Printing (HPLIP) system
provides a unified single and multi-function connectivity
driver solution. HPLIPLITE provides support for print and scan only.
HPLIPFULL provides support for print, scan, fax and toolbox.
%files
%attr(0644,root,root) "/etc/hp/hplip.conf"
%attr(0644,root,root) "/etc/udev/rules.d/56-hpmud.rules"
%attr(0755,root,root) "/usr/lib/cups/filter/hpcups"
%attr(0755,root,root) "/usr/lib/cups/filter/hpps"
%attr(0644,root,root) "/usr/share/cups/drv/hp/hpcups.drv"
%attr(0775,root,root) %dir "/usr/share/hplip/data/models"
%attr(0644,root,root) "/usr/share/hplip/data/models/models.dat"
%attr(0755,root,root) "/usr/share/hplip/locatedriver"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-LJ-Class1.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-LJ-Class2.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-LJ-Class3.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-LJ-Class4.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-LJ-Class4A.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-LJ-Class5.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-LJ-Class6.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL3-Class1.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL3-Class1A.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL3-Class1B.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL3-Class2.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL3-Class3.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL3-Class3A.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL3-Class3B.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL4-Class1.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCLM.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-postscript-inkjet.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-postscript-laserjet-pro.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-postscript-laserjet.ppd.gz"
