@echo off

if [%1]==[] goto usage
if [%2]==[] goto usage
if [%3]==[] goto usage
if [%4]==[] goto usage
if [%5]==[] goto usage

::set SolutionDir = %1
::set PlatformTarget = %2
::set OutputPath = %3
::set ProjectDir = %4
::set Configuration = %5

for /R %4..\libs\SDL2-2.0.12\lib\%2 %%f in (*.dll) do copy /v /y %%f %3
for /R %4..\libs\SDL2_ttf-2.0.15\lib\%2 %%f in (*.dll) do copy /v /y %%f %3
for /R %4..\libs\SDL2_image-2.0.5\lib\%2 %%f in (*.dll) do copy /v /y %%f %3

rmdir /s %1\build\include

robocopy ..\libs\SDL2_ttf-2.0.15\include %1\build\include /e 
robocopy ..\libs\SDL2-2.0.12\include %1\build\include /e 
robocopy ..\libs\SDL2_image-2.0.5\include %1\build\include /e 
robocopy ..\libs\glm-0.9.9.7\glm %1\build\include\glm /e 
robocopy ..\libs\entt-3.3.2\src %1\build\include /e
robocopy ..\libs\glad\%5 %1\build\include /e
robocopy ..\libs\stb_image-2.25 %1\build\include /e
mklink /D %1\build\include\sgw %4\include
::robocopy include %1\build\include\sgw /e
goto :eof
:usage
@echo Usage: %0 ^<SolutionDir^> ^<PlatformTarget^> ^<OutputPath^> ^<ProjectDir^>
exit /B 1

::for /R $(SolutionDir)libs\SDL2-2.0.12\lib\$(PlatformTarget) %%f in (*.dll) do copy /v /y %%f $(OutputPath) 
::&& for /R $(SolutionDir)libs\SDL2_ttf-2.0.15\lib\$(PlatformTarget) %%f in (*.dll) do copy /v /y %%f $(OutputPath) 
::&& for /R $(SolutionDir)libs\SDL2_image-2.0.5\lib\$(PlatformTarget) %%f in (*.dll) do copy /v /y %%f $(OutputPath) 
::&& robocopy $(SolutionDir)assets $(TargetDir)assets /e 
::&& robocopy $(SolutionDir)libs\SDL2_ttf-2.0.15\include $(SolutionDir)build\include /e 
::&& robocopy $(SolutionDir)libs\SDL2-2.0.12\include $(SolutionDir)build\include /e 
::&& robocopy $(SolutionDir)libs\SDL2_image-2.0.5\include $(SolutionDir)build\include /e 
::&& robocopy $(SolutionDir)libs\glm-0.9.9.7 $(SolutionDir)build\include /e 
::&& robocopy $(SolutionDir)libs\entt-3.3.2\src $(SolutionDir)build\include /e