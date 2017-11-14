cd ../../../../
make web_config-clean
make web_config
cd ./output/target/var
tar cvf www.tar.gz ./www/
mv www.tar.gz ~/Desktop/sambadir

