folder=`ls luapkg`
if [ -z "$folder" ]; then
    echo "Cloning submodules"
    git submodule init
    git submodule update
fi
git submodule foreach git pull origin master
