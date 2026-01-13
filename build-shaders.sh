output_dir="build/"

if which qsb-qt6 2> /dev/null ; then
  export qsb="qsb-qt6"
elif which qsb 2> /dev/null ; then
  export qsb="qsb"
fi

if [[ -z "$qsb" ]] ; then
  echo "Qt qsb tool not found. Make sure you have the development package installed for Qt GUI."
  exit -1
fi

shader="src/game/hud/InteractionColorOverlay.shader"
echo "+ building $shader"
mkdir -p "$output_dir/$(dirname "$shader")"
echo "++ vert"
$qsb --glsl 100es,120,150 --hlsl 50 --msl 12 -b -o $output_dir/$shader.vert.qsb $shader.vert
echo "++ frag"
$qsb --glsl 100es,120,150 --hlsl 50 --msl 12 -b -o $output_dir/$shader.frag.qsb $shader.frag

shader="src/game/level/PlayerCropCircle.shader"
echo "+ building $shader"
mkdir -p "$output_dir/$(dirname "$shader")"
echo "++ vert"
$qsb --glsl 100es,120,150 --hlsl 50 --msl 12 -b -o $output_dir/$shader.frag.qsb $shader.frag
echo "++ frag"
$qsb --glsl 100es,120,150 --hlsl 50 --msl 12 -b -o $output_dir/$shader.vert.qsb $shader.vert
echo "+ shaders built"
