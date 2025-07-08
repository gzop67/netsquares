@echo off
if not exist build mkdir build
pushd build

cl /Fe"netsquares" /Od /MP /MDd /Zi /EHsc^
  ../impl/*.cpp^
  ../client/impl/*.cpp^
  user32.lib Gdi32.lib shell32.lib winmm.lib Ws2_32.lib

cl /Fe"server" /Od /MP /MDd /Zi /EHsc^
  ../server/impl/*.cpp^
  user32.lib Gdi32.lib shell32.lib winmm.lib Ws2_32.lib

popd
EXIT /b
