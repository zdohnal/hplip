#! /bin/sh
#
# (c) Copyright 2003-2015 HP Development Company, L.P.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# Author: 
#
#echo "HP Linux Imaging and Printing System createPPD file"
make -f Makefile_dat2drv
rip="foomatic-rip-hplip"
for argument in $*;do

   if [ "$argument" = "-f" ]
   then
	classDriver="-f"
        ./Dat2drv $classDriver
  
   elif [ "$argument" = "-q" ]
   then
	./Dat2drv
   fi

done
ppd_version=`grep AC_INIT configure.in | awk '{print $7}' | sed 's/.*\[\(.*\)\].*/\1/'|egrep -o '[[:digit:]]{1,}\.[[:digit:]]{1,}\.[[:digit:]]{1,}'`
sed -e "s/@VERSION@/$ppd_version/g;s/@foomatic_filter@/$rip/g" prnt/drv/hpijs.drv.in >prnt/drv/hpijs.drv
sed -e "s/@VERSION@/$ppd_version/g" prnt/drv/hpcups.drv.in >prnt/drv/hpcups.drv

# disabling the removal of ppd folder
#rm -fr ppd

if [ "$classDriver" = "-f" ] # class drivers
then
	echo $PWD
	install -d ppd/classppd
	install -d ppd/classppd/hpcups 
	install -d ppd/classppd/ps
	ppdc -d ppd/classppd/hpcups prnt/drv/hpcups.drv
    	if [ -f "prnt/ps/hp-postscript-inkjet.ppd.gz" ]
	then
		cp prnt/ps/hp-postscript*.ppd.gz ppd/classppd/ps
	else
		cp prnt/ps/hp-postscript*.ppd ppd/classppd/ps
		for i in ppd/classppd/ps/*.ppd; do
			gzip -c "$i" > "$i.gz"
		done
	fi
        #cp prnt/ps/hp-postscript*.ppd.gz ppd/classppd/ps
	for i in ppd/classppd/hpcups/*.ppd; do
		gzip -c "$i" > "$i.gz"
	done
	#for i in ppd/classppd/ps/*.ppd; do
	#	gzip -c "$i" > "$i.gz"
	#done

	cat > class_cups_drv.inc <<EOF
class_cups_ppd_printers = \\
EOF
	for i in ppd/classppd/hpcups/*.ppd.gz; do
		echo "   $i \\" >>class_cups_drv.inc 
	done
        for i in ppd/classppd/ps/*.ppd.gz; do
                echo "   $i \\" >>class_cups_drv.inc
        done
	sed -i -e "s|$i .$|$i|" class_cups_drv.inc
        echo "cups_drv = prnt/drv/hpcups.drv" >> class_cups_drv.inc
        #exit 0
else # discrete drivers
install -d ppd/hpcups
ppdc -d ppd/hpcups prnt/drv/hpcups.drv
for i in ppd/hpcups/*.ppd; do
      gzip -c "$i" > "$i.gz"
done

install -d ppd/hpijs
ppdc -d ppd/hpijs prnt/drv/hpijs.drv
echo "gzip ppd files..."
for i in ppd/hpijs/*.ppd; do
      gzip -c "$i" > "$i.gz"
done
echo "building foomatic_drv.inc..."
./hpijs-drv > foomatic_drv.inc $shadow

echo "building cups_drv.inc..."
cat > cups_drv.inc <<EOF
cups_ppd_printers = \\
EOF
for i in ppd/hpcups/*.ppd.gz; do
      echo "   $i \\" >>cups_drv.inc 
done     
sed -i -e "s|$i .$|$i|" cups_drv.inc
echo "cups_drv = prnt/drv/hpcups.drv" >>cups_drv.inc
fi

# Update Makefile after PPD generate based on family or discrete
#libtoolize --force
#aclocal
#automake --foreign --add-missing
#autoconf
UNAME='uname -m'
VER=`$UNAME`

cd prnt/hpcups
if [ "$VER" = "x86_64" ] 
then
	ln -sf libImageProcessor-x86_64.so libImageProcessor.so
else
	ln -sf libImageProcessor-x86_32.so libImageProcessor.so
fi
cd ../../

exit 0
