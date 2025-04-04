export const MouseMode  = { "Movement": 0, "Interaction": 1, "Target": 2, "Wait": 3 };
export const TargetMode = { "Any": 0, "Character": 1, "Zone": 2 };

export function shouldDisplayInteractionLayer(mouseMode, targetMode) {
  switch (mouseMode)
  {
  case MouseMode.Target:
    if (targetMode !== TargetMode.Any)
      return false;
  case MouseMode.Interaction:
    return true;
  default:
    break ;
  }
  return false;
}

export function shouldDisplayTargetLayer(mouseMode, targetMode) {
  return mouseMode === MouseMode.Target && targetMode !== TargetMode.Any;
}
