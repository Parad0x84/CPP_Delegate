@echo off

pushd ..
Script\Premake\premake5.exe --file=Premake.lua vs2022
popd

IF %ERRORLEVEL% NEQ 0 (
  PAUSE
)
