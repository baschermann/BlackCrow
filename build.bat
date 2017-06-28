:: Build BWAPI
pushd %CD%
cd bwapi\bwapi
msbuild /p:Configuration=Debug   /p:Platform=Win32 /target:BWAPI bwapi.sln
msbuild /p:Configuration=Release /p:Platform=Win32 /target:BWAPI bwapi.sln

msbuild /p:Configuration=Debug   /p:Platform=Win32 /target:BWAPIClient bwapi.sln
msbuild /p:Configuration=Release /p:Platform=Win32 /target:BWAPIClient bwapi.sln
popd

:: Build KBot
msbuild /p:Configuration=Debug   /p:Platform=Win32 Crow.sln
msbuild /p:Configuration=Release /p:Platform=Win32 Crow.sln
