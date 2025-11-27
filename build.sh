# Check the root
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: CMakeLists.txt not found. Please run this script from the MiniFileExplorer root directory."
    exit 1
fi

# Run CMake to configure the project and check for errors
echo "Configuring the project with CMake..."
cmake ..
if [ $? -ne 0 ]; then
    echo "CMake configuration failed. Please check the output for errors."
    exit 1
fi

# make the project
echo "Building the project..."
make
if [ $? -ne 0 ]; then
    echo "Build failed. Please check the output for errors."
    exit 1
fi

echo "Build completed successfully."
exit 0
