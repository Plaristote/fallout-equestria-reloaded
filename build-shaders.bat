setlocal EnableDelayedExpansion
SET "output_dir=build/"
SET "qsb=C:\Qt\6.8.1\llvm-mingw_64\bin\qsb.exe"
SET "shader=src\game\hud\InteractionColorOverlay.shader"
echo "+ building %shader%"
for %%F in ("%shader%") do set dirname=%%~dpF
MKDIR "%output_dir%/%dirname%"
ECHO "++ vert"
%qsb% "--glsl" "100es,120,150" "--hlsl" "50" "--msl" "12" "-b" "-o" "%output_dir%\%shader%.vert.qsb" "%shader%.vert"
ECHO "++ frag"
%qsb% "--glsl" "100es,120,150" "--hlsl" "50" "--msl" "12" "-b" "-o" "%output_dir%\%shader%.frag.qsb" "%shader%.frag"
SET "shader=src\game\level\PlayerCropCircle.shader"
ECHO "+ building %shader%"
for %%F in ("%shader%") do set dirname=%%~dpF
MKDIR "%output_dir\%dirname%"
echo "++ vert"
%qsb% "--glsl" "100es,120,150" "--hlsl" "50" "--msl" "12" "-b" "-o" "%output_dir%\%shader%.frag.qsb" "%shader%.frag"
%qsb% "--glsl" "100es,120,150" "--hlsl" "50" "--msl" "12" "-b" "-o" "%output_dir%\%shader%.vert.qsb" "%shader%.vert"
