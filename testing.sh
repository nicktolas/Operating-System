if [ -d "./build" ] 
then
    echo "cleaning up last run" 
    make clean
fi 

make all
sh launch_gdb.sh &
make run