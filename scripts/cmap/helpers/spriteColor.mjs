const availableSpriteColors = [
  ["purple", {r: 165, g: 0, b: 128}],
  ["green",  {r: 165, g: 103, b: 0}],
  ["black",  {r: 29, g: 29, b: 29}],
  ["gray",   {r: 165, g: 165, b: 165}],
  ["orange", {r: 255, g: 165, b: 0}]
]

function square(a) { return a * a }

export function getSpriteColor(color) {
  var bestScore = -1;
  var candidate = "purple";

  for (var i = 0 ; i < availableSpriteColors.length ; ++i) {
    const otherColor = availableSpriteColors[i][1];
    const score = Math.sqrt(
      square(color.r * 255 - otherColor.r) +
      square(color.g * 255 - otherColor.g) +
      square(color.b * 255 - otherColor.b)
    );

    if (score < bestScore || bestScore === -1) {
      bestScore = score;
      candidate = availableSpriteColors[i][0];
    }
  }
  return candidate;
}
