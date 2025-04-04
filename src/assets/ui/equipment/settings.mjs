export const layouts = {
  pony: {
    "source": "pony.png",
    "use-1":  { x: 0,   y: 222 },
    "use-2":  { x: 349, y: 222 },
    "armor":  { x: 0,   y: 0 },
    "saddle": { x: 349, y: 0 }
  }
};

export function layoutFor(character) {
  switch (character.statistics.race) {
  case "earth-pony":
  case "unicorn":
  case "pegasus":
    return layouts.pony;
  }
  return null;
}
