@echo off
set OutputDir=out
set TargetDir=x64\Release
rmdir /s /q %OutputDir% 2>nul
mkdir %OutputDir%

echo 复制主程序及依赖...
xcopy /e /y %TargetDir%\*.exe %OutputDir%\
xcopy /e /y %TargetDir%\*.dll %OutputDir%\
xcopy /e /y %TargetDir%\*.json %OutputDir%\

echo 复制资源文件...
xcopy /e /y Resources\* %OutputDir%\Resources\

echo 打包完成，输出目录: %OutputDir%