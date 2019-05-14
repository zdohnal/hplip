Name: hplipclassdriver
Version: 3.19.5
Release: 0
License: (c) 2004-2009 Copyright HP Development Company, LP
Packager: HPLIP
Vendor: HP
Summary: HPLIP
Group: Applications
%post
ldconfig
semodule -i /usr/share/hplip/selinux/mypol.pp
#ln -sf /usr/lib/libImageProcessor-x86_64.so /usr/lib/libImageProcessor.so
ln -sf /usr/lib/libImageProcessor-x86_32.so /usr/lib/libImageProcessor.so
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
%attr(0644,root,root) "/usr/share/ppd/HP/hp-DJ9xxVIP.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-CLE.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-CLE17.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Ampere.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Copperhead.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Copperhead12.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-CopperheadIPH.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-CopperheadIPH15.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-CopperheadIPH17.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Corbett.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-CopperheadXLP.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-DJGenericVIP.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-DJ55xx.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Kapan.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Gemstone.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Mimas.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Mimas15.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Mimas17.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-MimasTDR.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-P15_CISS.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-OJProKx50.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-OJ7000.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PSP100.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Pyramid.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Pyramid15.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PSP470.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PyramidPlus.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PyramidRefresh17.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PyramidRefresh15.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Python.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Python10.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-SPDOfficejetProBsize.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-SPDOfficejetProAsize.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Python11.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Stabler.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Saipan.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-Saipan15B.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-ViperPlusVIP.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-ViperMinusVIP.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-StingrayOJ.ppd.gz"
#%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL3-Class1.ppd.gz"
#%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL3-Class1A.ppd.gz"
#%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL3-Class1B.ppd.gz"
#%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL3-Class2.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL3-Class3.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL3-Class3A.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL3-Class3B.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCL4-Class1.ppd.gz"
#%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCLM.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCLM_COLOR.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-PCLM_MONO.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-postscript-inkjet.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-postscript-laserjet-pro.ppd.gz"
%attr(0644,root,root) "/usr/share/ppd/HP/hp-postscript-laserjet.ppd.gz"
%attr(0775,root,root) %dir "/usr/share/hplip/selinux"
%attr(0755,root,root) "/usr/share/hplip/selinux/hplip.fc"
%attr(0755,root,root) "/usr/share/hplip/selinux/hplip.if"
%attr(0755,root,root) "/usr/share/hplip/selinux/hplip.te"
%attr(0755,root,root) "/usr/share/hplip/selinux/hplip.pp"
%attr(0755,root,root) "/usr/share/hplip/selinux/mypol.te"
%attr(0755,root,root) "/usr/share/hplip/selinux/mypol.pp"
%attr(0775,root,root) %dir "/usr/lib/"
%attr(0775,root,root) "/usr/lib/libImageProcessor-x86_64.so"
%attr(0775,root,root) "/usr/lib/libImageProcessor-x86_32.so"
