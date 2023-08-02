output_dir="build/"
shader_list="$shader_list game/level/Daylight"
shader_list="$shader_list game/level/PlayerCropCircle.shader"
shader_list="$shader_list game/hud/InteractionColorOverlay.shader"
qsb="qsb-qt6"

for shader in $shader_list ; do
  echo "+ building $shader"
  mkdir -p "$output_dir/$(dirname "$shader")"
  echo "++ vert"
  $qsb --glsl 100es,120,150 --hlsl 50 --msl 12 -b -o $output_dir/$shader.vert.qsb $shader.vert
  echo "++ frag"
  $qsb --glsl 100es,120,150 --hlsl 50 --msl 12 -b -o $output_dir/$shader.frag.qsb $shader.frag
done
