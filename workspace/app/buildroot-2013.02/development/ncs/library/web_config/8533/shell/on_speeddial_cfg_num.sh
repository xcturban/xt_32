x2i()
{
  cp /var/www/x2i/speeddial_cfg.x2i /var/www/x2i/speeddial_cfg_$1.x2i
  find /var/www/x2i/ -name speeddial_cfg_$1.x2i -exec sed -i -r "s/<\[num\]>/$1/g" {} \;
}

for index in 1 2 3 4 ;
do
x2i $index;
done

