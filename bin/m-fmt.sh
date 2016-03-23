#!/bin/bash
function fmt
{
	sed -e 's/^\t$//g' -i $1
	sed -e "s/[ \t]*$//g" -i $1
	t=$1
	t=${t##*.}
	if [ "$t" == "c" ]; then
		indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0 $1
	fi
	if [ "$t" == "h" ]; then
		if cat $1 | grep __ASSEMBLY__; then
			echo "skip $1"
		else
			indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0 $1
		fi
	fi
}

dir=$1
if [ ! -d "$dir" ]; then
	echo "usage $0 <dir to scan>"
	return
fi

for f in $(find $dir -name "*.[hHSc]") ; do
	sed -e 's/^\t$//g' -i $f
	sed -e "s/[ \t]*$//g" -i $f
done

for f in $(find $dir -name "*.ld") ; do
	sed -e 's/^\t$//g' -i $f
	sed -e "s/[ \t]*$//g" -i $f
done

for f in $(find $dir -name "*.[hHc]") ; do
	fmt $f
done
