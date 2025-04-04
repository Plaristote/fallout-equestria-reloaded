output_dir="build/"
qsb="qsb-qt6"

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
