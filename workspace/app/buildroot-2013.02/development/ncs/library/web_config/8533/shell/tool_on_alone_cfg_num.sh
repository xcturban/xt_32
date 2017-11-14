x2i()
{
  cp ../x2i/quickcall_cfg.x2i ../x2i/quickcall_cfg_$1.x2i
  find ../x2i/ -name quickcall_cfg_$1.x2i -exec sed -i -r "s/<\[num\]>/$1/g" {} \;
}

for index in 1 2 ;
do
x2i $index;
done
