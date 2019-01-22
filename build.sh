echo "Building File Transfer Program";

# Compile Server and Client
gcc -o server -pthread source/server.c source/utils.c && \
gcc -o client source/client.c source/utils.c 

rc=$?; if [[ $rc != 0 ]]; then echo "Compilation Failed\n" && exit $rc; fi

# Create Directory for Server and Client Executable
mkdir server_dir
mkdir client_dir
mv server server_dir
mv client client_dir

# Create test file for server
cp test_files/* server_dir

echo "Build Complete..!\n"
