x2i()
{
  cp /var/www/x2i/board_cfg.x2i /var/www/x2i/board_cfg_$1.x2i
  find /var/www/x2i/ -name board_cfg_$1.x2i -exec sed -i -r "s/<\[num\]>/$1/g" {} \;
}

for index in 1 2 3 4 ;
do
x2i $index;
done

